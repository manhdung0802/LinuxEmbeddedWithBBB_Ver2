
#include <GLFW/glfw3.h>
#include <config.h>
#ifdef USER_ADDER
#include <adder.h>
#endif
#include <iostream>

int main() {
#ifdef USER_ADDER
	std::cout << "Using libs - Manh Dung " << dung::add(4, 5) << std::endl;
#endif

	std::cout << "Starting" << std::endl;
	GLFWwindow *window;

	if (!glfwInit()) {
		std::cout << "Error" << std::endl;
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(300, 300, "Gears", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to open GLFW window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	// Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Terminate GLFW
	glfwTerminate();

	return 0;
}