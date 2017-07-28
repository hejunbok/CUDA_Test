#include "funset.hpp"
#include <random>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "common.hpp"
#include <opencv2/opencv.hpp>

int test_heat_conduction()
{
	const int width{ 1024 }, height = width;
	cv::Mat mat1(height, width, CV_8UC4), mat2(height, width, CV_8UC4);

	const float speed{ 0.25f }, max_temp{ 1.f }, min_temp{0.0001f};
	float elapsed_time1{ 0.f }, elapsed_time2{ 0.f }; // milliseconds

	// intialize the constant data
	std::unique_ptr<float[]> temp(new float[width * height]);
	for (int i = 0; i < width*height; ++i) {
		temp[i] = 0;
		int x = i % width;
		int y = i / height;
		if ((x>300) && (x<600) && (y>310) && (y<601))
			temp[i] = max_temp;
	}

	temp[width * 100 + 100] = (max_temp + min_temp) / 2;
	temp[width * 700 + 100] = min_temp;
	temp[width * 300 + 300] = min_temp;
	temp[width * 200 + 700] = min_temp;

	for (int y = 800; y < 900; ++y) {
		for (int x = 400; x < 500; ++x) {
			temp[x + y * width] = min_temp;
		}
	}

	int ret = heat_conduction_cpu(mat1.data, width, height, temp.get(), speed, &elapsed_time1);
	if (ret != 0) PRINT_ERROR_INFO(heat_conduction_cpu);

	ret = heat_conduction_gpu(mat2.data, width, height, temp.get(), speed, &elapsed_time2);
	if (ret != 0) PRINT_ERROR_INFO(heat_conduction_gpu);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			cv::Vec4b val1 = mat1.at<cv::Vec4b>(y, x);
			cv::Vec4b val2 = mat2.at<cv::Vec4b>(y, x);

			for (int i = 0; i < 4; ++i) {
				if (val1[i] != val2[i]) {
					fprintf(stderr, "their values are different at (%d, %d), i: %d, val1: %d, val2: %d\n",
						x, y, i, val1[i], val2[i]);
					//return -1;
				}
			}
		}
	}

	std::string save_image_name{ "E:/GitCode/CUDA_Test/heat_conduction.jpg" };
	cv::resize(mat2, mat2, cv::Size(width / 2, height / 2), 0.f, 0.f, 2);
	cv::imwrite(save_image_name, mat2);

	fprintf(stderr, "test heat conduction: cpu run time: %f ms, gpu run time: %f ms\n", elapsed_time1, elapsed_time2);

	return 0;
}

int test_ray_tracking()
{
	const int spheres{ 20 };
	std::unique_ptr<float[]> A(new float[spheres * 3]);
	std::unique_ptr<float[]> B(new float[spheres * 3]);
	std::unique_ptr<float[]> C(new float[spheres]);

	generator_random_number(A.get(), spheres * 3, 0.f, 1.f);
	generator_random_number(B.get(), spheres * 3, -400.f, 400.f);
	generator_random_number(C.get(), spheres, 20.f, 120.f);

	float elapsed_time1{ 0.f }, elapsed_time2{ 0.f }; // milliseconds

	const int width{ 512 }, height = width;
	cv::Mat mat1(height, width, CV_8UC4), mat2(height, width, CV_8UC4);

	int ret = ray_tracking_cpu(A.get(), B.get(), C.get(), spheres, mat1.data, width, height, &elapsed_time1);
	if (ret != 0) PRINT_ERROR_INFO(ray_tracking_cpu);

	ret = ray_tracking_gpu(A.get(), B.get(), C.get(), spheres, mat2.data, width, height, &elapsed_time2);
	if (ret != 0) PRINT_ERROR_INFO(ray_tracking_gpu);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			cv::Vec4b val1 = mat1.at<cv::Vec4b>(y, x);
			cv::Vec4b val2 = mat2.at<cv::Vec4b>(y, x);

			for (int i = 0; i < 4; ++i) {
				if (val1[i] != val2[i]) {
					fprintf(stderr, "their values are different at (%d, %d), i: %d, val1: %d, val2: %d\n",
						x, y, i, val1[i], val2[i]);
					//return -1;
				}
			}
		}
	}

	const std::string save_image_name{ "E:/GitCode/CUDA_Test/ray_tracking.jpg" };
	cv::imwrite(save_image_name, mat2);

	fprintf(stderr, "ray tracking: cpu run time: %f ms, gpu run time: %f ms\n", elapsed_time1, elapsed_time2);

	return 0;
}

int test_green_ball()
{
	const int width{ 512 }, height = width;
	cv::Mat mat1(height, width, CV_8UC4), mat2(height, width, CV_8UC4);

	float elapsed_time1{ 0.f }, elapsed_time2{ 0.f }; // milliseconds

	int ret = green_ball_cpu(mat1.data, width, height, &elapsed_time1);
	if (ret != 0) PRINT_ERROR_INFO(green_ball_cpu);

	ret = green_ball_gpu(mat2.data, width, height, &elapsed_time2);
	if (ret != 0) PRINT_ERROR_INFO(green_ball_gpu);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			cv::Vec4b val1 = mat1.at<cv::Vec4b>(y, x);
			cv::Vec4b val2 = mat2.at<cv::Vec4b>(y, x);

			for (int i = 0; i < 4; ++i) {
				if (val1[i] != val2[i]) {
					fprintf(stderr, "their values are different at (%d, %d), i: %d, val1: %d, val2: %d\n",
						x, y, i, val1[i], val2[i]);
					//return -1;
				}
			}
		}
	}

	const std::string save_image_name{ "E:/GitCode/CUDA_Test/gree_ball.jpg" };
	cv::imwrite(save_image_name, mat2);

	fprintf(stderr, "test green ball: cpu run time: %f ms, gpu run time: %f ms\n", elapsed_time1, elapsed_time2);

	return 0;
}

int test_ripple()
{
	const int width{ 512 }, height = width;
	const int ticks{ 999 };
	cv::Mat mat1(height, width, CV_8UC4), mat2(height, width, CV_8UC4);

	float elapsed_time1{ 0.f }, elapsed_time2{ 0.f }; // milliseconds

	int ret = ripple_cpu(mat1.data, width, height, ticks, &elapsed_time1);
	if (ret != 0) PRINT_ERROR_INFO(ripple_cpu);

	ret = ripple_gpu(mat2.data, width, height, ticks, &elapsed_time2);
	if (ret != 0) PRINT_ERROR_INFO(ripple_gpu);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			cv::Vec4b val1 = mat1.at<cv::Vec4b>(y, x);
			cv::Vec4b val2 = mat2.at<cv::Vec4b>(y, x);

			for (int i = 0; i < 4; ++i) {
				if (val1[i] != val2[i]) {
					fprintf(stderr, "their values are different at (%d, %d), i: %d, val1: %d, val2: %d\n",
						x, y, i, val1[i], val2[i]);
					return -1;
				}
			}
		}
	}

	const std::string save_image_name{ "E:/GitCode/CUDA_Test/ripple.jpg" };
	cv::imwrite(save_image_name, mat2);

	fprintf(stderr, "cpu run time: %f ms, gpu run time: %f ms\n", elapsed_time1, elapsed_time2);

	return 0;
}

int test_julia()
{
	const int width{ 512 }, height = width;
	const float scale{ 1.5f };
	cv::Mat mat1(height, width, CV_8UC4), mat2(height, width, CV_8UC4);

	float elapsed_time1{ 0.f }, elapsed_time2{ 0.f }; // milliseconds

	int ret = julia_cpu(mat1.data, width, height, scale, &elapsed_time1);
	if (ret != 0) PRINT_ERROR_INFO(julia_cpu);

	ret = julia_gpu(mat2.data, width, height, scale, &elapsed_time2);
	if (ret != 0) PRINT_ERROR_INFO(julia_gpu);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			cv::Vec4b val1 = mat1.at<cv::Vec4b>(y, x);
			cv::Vec4b val2 = mat2.at<cv::Vec4b>(y, x);

			for (int i = 0; i < 4; ++i) {
				if (val1[i] != val2[i]) {
					fprintf(stderr, "their values are different at (%d, %d), i: %d, val1: %d, val2: %d\n",
						x, y, i, val1[i], val2[i]);
					//return -1;
				}
			}
		}
	}

	const std::string save_image_name{ "E:/GitCode/CUDA_Test/julia.jpg" };
	cv::imwrite(save_image_name, mat2);

	fprintf(stderr, "cpu run time: %f ms, gpu run time: %f ms\n", elapsed_time1, elapsed_time2);

	return 0;
}

int test_dot_product()
{
	const int length{ 10000000 };
	std::unique_ptr<float[]> A(new float[length]);
	std::unique_ptr<float[]> B(new float[length]);

	generator_random_number(A.get(), length, -10.f, 10.f);
	generator_random_number(B.get(), length, -10.f, 10.f);

	float elapsed_time1{ 0.f }, elapsed_time2{ 0.f }; // milliseconds
	float value1{ 0.f }, value2{ 0.f };

	int ret = dot_product_cpu(A.get(), B.get(), &value1, length, &elapsed_time1);
	if (ret != 0) PRINT_ERROR_INFO(long_vector_add_cpu);

	ret = dot_product_gpu(A.get(), B.get(), &value2, length, &elapsed_time2);
	if (ret != 0) PRINT_ERROR_INFO(matrix_mul_gpu);

	if (fabs(value1 - value2) > EPS_) {
		fprintf(stderr, "Result verification failed value1: %f, value2: %f\n", value1, value2);
	}

	fprintf(stderr, "cpu run time: %f ms, gpu run time: %f ms\n", elapsed_time1, elapsed_time2);

	return 0;
}

int test_long_vector_add()
{
	const int length{ 100000000 };
	std::unique_ptr<float[]> A(new float[length]);
	std::unique_ptr<float[]> B(new float[length]);
	std::unique_ptr<float[]> C1(new float[length]);
	std::unique_ptr<float[]> C2(new float[length]);

	generator_random_number(A.get(), length, -1000.f, 1000.f);
	generator_random_number(B.get(), length, -1000.f, 1000.f);

	float elapsed_time1{ 0.f }, elapsed_time2{ 0.f }; // milliseconds
	int ret = long_vector_add_cpu(A.get(), B.get(), C1.get(), length, &elapsed_time1);
	if (ret != 0) PRINT_ERROR_INFO(long_vector_add_cpu);

	ret = long_vector_add_gpu(A.get(), B.get(), C2.get(), length, &elapsed_time2);
	if (ret != 0) PRINT_ERROR_INFO(matrix_mul_gpu);

	int count_error{ 0 };
	for (int i = 0; i < length; ++i) {
		if (count_error > 100) return -1;
		if (fabs(C1[i] - C2[i]) > EPS_) {
			fprintf(stderr, "Result verification failed at element %d, C1: %f, C2: %f\n",
				i, C1[i], C2[i]);
			++count_error;
		}
	}

	fprintf(stderr, "cpu run time: %f ms, gpu run time: %f ms\n", elapsed_time1, elapsed_time2);

	return 0;
}

int test_matrix_mul()
{
	// Matrix multiplication: C = A * B
	// 矩阵A、B的宽、高应是32的整数倍
	const int rowsA{ 352 }, colsA{ 672 }, rowsB = colsA, colsB{ 384 };
	std::unique_ptr<float[]> A(new float[colsA*rowsA]);
	std::unique_ptr<float[]> B(new float[colsB*rowsB]);
	std::unique_ptr<float[]> C1(new float[rowsA*colsB]);
	std::unique_ptr<float[]> C2(new float[rowsA*colsB]);

	generator_random_number(A.get(), colsA*rowsA, -1.f, 1.f);
	generator_random_number(B.get(), colsB*rowsB, -1.f, 1.f);

	float elapsed_time1{ 0.f }, elapsed_time2{ 0.f }; // milliseconds
	int ret = matrix_mul_cpu(A.get(), B.get(), C1.get(), colsA, rowsA, colsB, rowsB, &elapsed_time1);
	if (ret != 0) PRINT_ERROR_INFO(matrix_mul_cpu);

	ret = matrix_mul_gpu(A.get(), B.get(), C2.get(), colsA, rowsA, colsB, rowsB, &elapsed_time2);
	if (ret != 0) PRINT_ERROR_INFO(matrix_mul_gpu);

	int count{ 0 };
	for (int i = 0; i < rowsA*colsB; ++i) {
		if (count > 100) return -1;
		if (fabs(C1[i] - C2[i]) > EPS_) {
			fprintf(stderr, "Result verification failed at element %d, C1: %f, C2: %f\n",
				i, C1[i], C2[i]);
			++count;
		}
	}

	fprintf(stderr, "cpu run time: %f ms, gpu run time: %f ms\n", elapsed_time1, elapsed_time2);

	return 0;
}

int test_vector_add()
{
	// Vector addition: C = A + B, implements element by element vector addition
	const int numElements{ 50000 };
	std::vector<float> A(numElements), B(numElements), C1(numElements), C2(numElements);

	// Initialize vector
	for (int i = 0; i < numElements; ++i) {
		A[i] = rand() / (float)RAND_MAX;
		B[i] = rand() / (float)RAND_MAX;
	}

	float elapsed_time1{ 0.f }, elapsed_time2{ 0.f }; // milliseconds
	int ret = vector_add_cpu(A.data(), B.data(), C1.data(), numElements, &elapsed_time1);
	if (ret != 0) PRINT_ERROR_INFO(vectorAdd_cpu);

	ret = vector_add_gpu(A.data(), B.data(), C2.data(), numElements, &elapsed_time2);
	if (ret != 0) PRINT_ERROR_INFO(vectorAdd_gpu);

	for (int i = 0; i < numElements; ++i) {
		if (fabs(C1[i] - C2[i]) > EPS_) {
			fprintf(stderr, "Result verification failed at element %d!\n", i);
			return -1;
		}
	}

	fprintf(stderr, "cpu run time: %f ms, gpu run time: %f ms\n", elapsed_time1, elapsed_time2);

	return 0;
}

