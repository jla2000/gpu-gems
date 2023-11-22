const std = @import("std");
const c = @cImport({
    @cInclude("GL/glew.h");
    @cInclude("GLFW/glfw3.h");
});

fn glfwErrorHandler(_: c_int, error_message: [*c]const u8) callconv(.C) void {
    std.debug.panic("GLFW error: {s}", .{error_message});
}

pub fn main() !void {
    _ = c.glfwSetErrorCallback(glfwErrorHandler);
    if (c.glfwInit() == c.GLFW_FALSE) {
        std.debug.panic("Failed to initialize GLEW.\n", .{});
    }
    defer c.glfwTerminate();

    c.glfwWindowHint(c.GLFW_OPENGL_PROFILE, c.GLFW_OPENGL_CORE_PROFILE);
    c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MAJOR, 4);
    c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MINOR, 4);

    const window = c.glfwCreateWindow(800, 600, "gpu-gems", null, null);
    if (window == null) {
        std.debug.panic("Failed to create window.\n", .{});
    }

    c.glfwMakeContextCurrent(window);

    const glewErr = c.glewInit();
    if (glewErr != c.GLEW_OK) {
        std.debug.panic("Failed to initialize GLEW: {s}.\n", .{c.glewGetErrorString(glewErr)});
    }

    while (c.glfwWindowShouldClose(window) == c.GLFW_FALSE) {
        c.glfwSwapBuffers(window);
        c.glfwPollEvents();
    }
}
