const std = @import("std");
const Allocator = std.mem.Allocator;

const TokenKind = enum(u4) {
    KEYWORD,
    IDENTIFIER,
    INTEGER,
    FLOAT,
    STRING,
};

const TokenValue = union(enum) {
    int: u64,
    float: f64,
    string: std.ArrayList(u8),
};

const Token = struct {
    kind: TokenKind,
    value: TokenValue,
    line: u32,
    begin: u32,
    end: u32,
};

const LexerError = error{
    UnexpectedCharacter,
    NumberTooBig,
};

pub fn Lexer() type {
    return struct {
        const Self = @This();
        index: u32 = 0,
        line: u32 = 1,
        char_index: u32 = 1,
        char: u8,
        eof: bool = false,
        code: *[]const u8,
        allocator: Allocator,
        keywords: std.StringHashMap(bool),

        pub fn init(allocator: Allocator, code: *[]const u8) !Self {
            var self = Self{
                .char = code.*[0],
                .code = code,
                .allocator = allocator,
                .keywords = std.StringHashMap(bool).init(allocator),
            };

            try self.keywords.put("if", true);
            try self.keywords.put("else", true);
            return self;
        }

        fn advance(self: *Self) void {
            self.index += 1;
            self.char_index += 1;
            self.eof = self.index >= self.code.*.len;
            self.char = if (self.eof) 0 else self.code.*[self.index];
            if (self.char == '\n') {
                self.line += 1;
                self.char_index = 1;
            }
        }

        fn isKeyword(self: *Self, identifier: []u8) bool {
            return self.keywords.get(@as([]const u8, identifier)) orelse false;
        }

        fn isIdentifier(self: *Self) bool {
            return switch (self.char) {
                'A'...'Z', 'a'...'z', '#', '_', '$' => true,
                else => false,
            };
        }

        fn getIdentifier(self: *Self) Allocator.Error!Token {
            const begin = self.char_index;
            var str = std.ArrayList(u8).init(self.allocator);
            errdefer str.deinit();

            while (self.isIdentifier()) {
                try str.append(self.char);
                self.advance();
            }

            str.shrinkAndFree(str.items.len);
            return Token{ .kind = if (self.isKeyword(str.items)) .KEYWORD else .IDENTIFIER, .value = .{ .string = str }, .line = self.line, .begin = begin, .end = self.index };
        }

        fn isNumber(self: *Self) bool {
            return switch (self.char) {
                '0'...'9', '.' => true,
                else => false,
            };
        }

        fn getNumber(self: *Self) LexerError!Token {
            const begin = self.char_index;
            var num: u64 = 0;
            var den: u6 = 1;
            var dotcount: u2 = 0;

            while (self.isNumber()) {
                if (self.char == '.') {
                    dotcount += 1;
                    if (dotcount > 1)
                        break;
                    self.advance();
                    continue;
                }

                num *= 10;
                try {
                    num += self.char - '0';
                } catch return LexerError.NumberTooBig;
                if (dotcount > 0)
                    den *= 10;
                self.advance();
            }

            if (dotcount > 0) {
                return Token{ .kind = .FLOAT, .value = .{ .float = @as(f64, @floatFromInt(num)) / @as(f64, @floatFromInt(den)) }, .line = self.line, .begin = begin, .end = self.index };
            }
            return Token{ .kind = .INTEGER, .value = .{ .int = num }, .line = self.line, .begin = begin, .end = self.index };
        }

        fn getString(self: *Self, char: u8) (Allocator.Error || LexerError)!Token {
            const line = self.line;
            const begin = self.char_index;
            var str = std.ArrayList(u8).init(self.allocator);
            errdefer str.deinit();

            self.advance();
            while (self.char != char) {
                if (self.eof)
                    return LexerError.UnexpectedCharacter;
                try str.append(self.char);
                self.advance();
            }
            self.advance();

            str.shrinkAndFree(str.items.len);
            return Token{ .kind = .STRING, .value = .{ .string = str }, .line = line, .begin = begin, .end = self.index };
        }

        fn doIgnore(self: *Self) bool {
            return switch (self.char) {
                ' ', '\n', '\t', '\r' => true,
                else => false,
            };
        }

        pub fn nextToken(self: *Self) (Allocator.Error || LexerError)!?Token {
            while (self.doIgnore()) {
                self.advance();
            }
            if (self.eof) {
                return null;
            }

            var token = (try self.allocator.create(Token)).*;
            errdefer self.allocator.destroy(&token);

            if (self.isNumber()) {
                token = try self.getNumber();
            } else if (self.isIdentifier()) {
                token = try self.getIdentifier();
            } else if (self.char == '"' or self.char == '\'') {
                token = try self.getString(self.char);
            } else {
                return LexerError.UnexpectedCharacter;
            }

            return token;
        }
    };
}
