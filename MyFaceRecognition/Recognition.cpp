#include "stdafx.h"
#include "Recognition.h"

Recognition::Recognition()
	:read_csv_file(false),
	load_sak_file(false),
	train(true),
	path_csv(".\\data\\at.csv"),
	path_train_datas(".\\data\\S_trainDatas.sak"),
	username("Mr.Lee"),
	images(),
	labels(),
	current_label(0),
	current_image(),
	error_msg("暂时没有错误信息"),
	model()
{
}

void Recognition::set_path_csv(const std::string &path)
{
	path_csv = path;
}

void Recognition::set_path_train_datas(const std::string &path)
{
	path_train_datas = path;
}

void Recognition::set_username(const std::string &name)
{
	username = name;
}

void Recognition::add_images(const cv::Mat &image)
{
	images.push_back(image);
}

void Recognition::add_labels(const int &number)
{
	labels.push_back(number);
}

void Recognition::set_current_image(const cv::Mat &cur_image)
{
	current_image = cur_image.clone();
}

void Recognition::set_current_image(const std::string &path)
{
	current_image = cv::imread(path, 0);
}

void Recognition::set_current_label(const int &num)
{
	current_label = num;
}

void Recognition::delete_image()
{
	images.pop_back();
}

void Recognition::delete_label()
{
	labels.pop_back();
}

void Recognition::clear_labels()
{
	labels.clear();
}

void Recognition::clear_images()
{
	images.clear();
}

std::string Recognition::get_path_csv() const
{
	return path_csv;
}

std::string Recognition::get_path_train_datas() const
{
	return path_train_datas;
}

std::string Recognition::get_username() const
{
	return username;
}

cv::Mat Recognition::get_current_image() const
{
	return current_image;
}

int Recognition::get_current_label() const
{
	return current_label;
}

std::string Recognition::get_error_msg() const
{
	return error_msg;
}

bool Recognition::need_train() const
{
	return train;
}

void Recognition::initialization()
{
	std::ifstream sak_file(path_train_datas, std::ifstream::in);
	if (!sak_file)	// 如果没有已经训练好的数据文件就进行新的训练
	{
		std::string tmpname;
		system("cls");
		std::cout << "没有训练数据，接下来进行初始化先：" << std::endl;
		std::cout << "请输入用户名称：" << std::endl;
		std::cin >> tmpname;
		set_username(tmpname);
		system("cls");
	}
	else
	{
		if (!load_sak_file)
		{
			model = cv::createEigenFaceRecognizer();
			system("cls");
			std::cout << "已有训练数据，加载中..." << std::endl;
			if (!read_csv_file)
			{
				read_csv(path_csv);	// 读取文件数据
			}
			model->load(path_train_datas);
			if (images.size() <= 1)	// 要求至少两个图片
			{
				error_msg = "该次训练至少要有两张图片！！";
				print_error_msg();
				return;
			}
			train = false;
			system("cls");
			std::cout << "加载完毕" << std::endl;
			load_sak_file = true;
		}
	}
}

void Recognition::read_csv(const std::string &fileName, const char separator)
{
	std::ifstream csv_file(fileName, std::ifstream::in);
	if (!csv_file)
	{
		error_msg = "找不到有效的文件";
		print_error_msg();
		return;
	}
	std::string s_line, s_path, s_number;	// s_line为整行的数据信息，s_path为路径名信息，s_number为标号信息
	while (std::getline(csv_file, s_line))
	{
		std::stringstream s_getLine(s_line);	//	string流s_getLine提供向string变量输入输出数据信息的功能
		std::getline(s_getLine, s_path, separator);	// 将从开始到";"的数据信息存入s_path中
		std::getline(s_getLine, s_number);	// 将剩余的数据信息存入到s_number中
		if (!s_path.empty() && !s_number.empty())
		{
			add_images(cv::imread(s_path, 0));	// 读取图片信息
			add_labels(atoi(s_number.c_str()));	//	读取标号信息，首先将string转char *，再将char *转为int；atoi函数是C语言的！
		}
	}
	csv_file.close();
	read_csv_file = true;
}

void Recognition::print_error_msg() const
{
	std::copy(error_msg.begin(), error_msg.end(), std::ostream_iterator<const char>(std::cout, ""));
}

int Recognition::predict_label() const
{
	return model->predict(current_image);
}

void Recognition::predict_label(int &label, double &value) const
{
	model->predict(current_image, label, value);
}

void Recognition::start_work()
{
	if (train)
	{// 需要重新训练
		model = cv::createEigenFaceRecognizer();
		system("cls");
		std::cout << "训练中..." << std::endl;
		if (!read_csv_file)
		{
			read_csv(path_csv);	// 读取文件数据
		}
		model->train(images, labels);	// 训练
		system("cls");
		std::cout << "训练结束，结果保存中..." << std::endl;
		model->save(path_train_datas);	// 保存训练结果
		train = false;
		system("cls");
		std::cout << "保存完毕" << std::endl;
		return;
	}
	current_label = predict_label();
	if (current_label == 41)	// 这里的41是用户指定的标号，也可以指定为其他的
	{
		system("cls");
		std::cout << "Hellow, " << username << std::endl;
		current_label = 0;	// 清0
	}
	else
	{
		system("cls");
		std::cout << "不对噢！" << std::endl;
		current_label = 0;	//	清0
	}
}
