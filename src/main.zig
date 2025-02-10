const std = @import("std");
const lx = @import("lexer.zig");

pub fn main() !void {
    var code: []const u8 = "123 12.34 \"Hello\" print '\"a'\"B\" if";
    var lexer = try lx.Lexer().init(std.heap.page_allocator, &code);

    while (try lexer.nextToken()) |token| {
        switch (token.kind) {
            .STRING => std.debug.print("<string:{s},line:{d}>", .{ token.value.string.items, token.line }),
            .KEYWORD => std.debug.print("<keyword:{s},line:{d}>", .{ token.value.string.items, token.line }),
            .IDENTIFIER => std.debug.print("<identifier:{s},line:{d}>", .{ token.value.string.items, token.line }),
            .INTEGER => std.debug.print("<integer:{d},line:{d}>", .{ token.value.int, token.line }),
            .FLOAT => std.debug.print("<float:{},line:{d}>", .{ token.value.float, token.line }),
        }
        std.debug.print("\n", .{});
    }
}
