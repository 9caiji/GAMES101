#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;
inline double DEG2RAD(double angle) { return angle * MY_PI / 180; }
Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    double angle = DEG2RAD(rotation_angle);
    model << cos(angle), -sin(angle), 0, 0,
        sin(angle), cos(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;

    return model;
}



Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
	Eigen::Matrix4f M_persp2ortho(4, 4);
	Eigen::Matrix4f M_ortho_scale(4, 4);
	Eigen::Matrix4f M_ortho_trans(4, 4);
	//已更正
	float angle = eye_fov * MY_PI / 180.0; 
	float height = zNear * tan(angle) * 2;
	float width = height * aspect_ratio;

	auto t = -zNear * tan(angle / 2);  // 上截面
	auto r = t * aspect_ratio;  //右截面   
	auto l = -r;  // 左截面
	auto b = -t;  // 下截面
	// 透视矩阵"挤压"
	M_persp2ortho << zNear, 0, 0, 0,
		0, zNear, 0, 0,
		0, 0, zNear + zFar, -zNear * zFar,
		0, 0, 1, 0;
	// 正交矩阵-缩放
    std::cout << 2/(zNear - zFar) << std::endl;
	M_ortho_scale << 2 / (r - l), 0, 0, 0,
		0, 2 / (t - b), 0, 0,
		0, 0, 2 / (zNear - zFar), 0,
		0, 0, 0, 1;
	// 正交矩阵-平移
	M_ortho_trans << 1, 0, 0, -(r + l) / 2,
		0, 1, 0, -(t + b) / 2,
		0, 0, 1, -(zNear + zFar) / 2,
		0, 0, 0, 1;
	Eigen::Matrix4f M_ortho = M_ortho_scale ;
	projection = M_ortho * M_persp2ortho * projection;

    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;
    double ang = 45;
    double znear = 0, zfar = 10;
    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(ang, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, znear, zfar));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }

        if (key == 'i') eye_pos[2] += 2;
        else if (key == 'k') eye_pos[2] -= 2;
        else if (key == 'y') znear += 0.5;
        else if (key == 'h') znear -= 0.5;
        else if (key == 'u') zfar += 5;
        else if (key == 'j') zfar -= 5;
        else if (key == 't') ang += 5;
        else if (key == 'g') ang -= 5;
        std::cout << ang << " " << znear <<" " <<zfar<< std::endl;
    }

    return 0;
}
