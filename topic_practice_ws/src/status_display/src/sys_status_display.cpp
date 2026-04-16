#include<QApplication>
#include<QLabel>
#include<QString>
#include <rclcpp/rclcpp.hpp>
#include <status_interfaces/msg/system_status.hpp>

using SystemStatus = status_interfaces::msg::SystemStatus;

class SysStatusDisplay : public rclcpp::Node
{
    private:
        rclcpp::Subscription<SystemStatus>::SharedPtr subscriber_;
        QLabel* label_;
    public:
        SysStatusDisplay() :Node("sys_status_display"){
            label_ = new QLabel();
            subscriber_ = this->create_subscription<SystemStatus>(
                "sys_status", 10, 
                [&](const SystemStatus::SharedPtr msg)->void{
                    label_ ->setText(get_qstring_from_msg(msg));
                }
            );
                    label_->setText(get_qstring_from_msg(std::make_shared<SystemStatus>()));
                    label_->show();
        };
        QString get_qstring_from_msg(const SystemStatus::SharedPtr msg){
            std::stringstream show_str;
            show_str << "==============新提供状态可视化工具==============\n"
             << "数 据 时 间：\t"<< msg ->stamp.sec <<"\ts\n"
             << "主 机 名 字：\t"<< msg ->host_name <<"\t\n"
             << "CPU 使 用 率：\t"<< msg ->cpu_percent <<"\t%\n"
             << "内 存 使 用 率：\t"<< msg ->memory_percent <<"\t%\n"
             << "内 存 总 大 小：\t"<< msg ->memory_total <<"\tMB\n"
             << "剩 余 有 效 内 存：\t"<< msg ->memory_available <<"\tMB\n"
             << "网 络 发 送 量：\t"<< msg ->net_sent <<"\tMB/s\n"
             << "网 络 接 收 量：\t"<< msg ->net_recv <<"\tMB/s\n"
             <<"==============================================================\n";

            return QString::fromStdString(show_str.str());
        }
};

int main(int argc, char* argv[]){
    rclcpp::init(argc, argv);
    QApplication app(argc, argv);
    auto node = std::make_shared<SysStatusDisplay>();
    std::thread spin_thread([&](){
        rclcpp::spin(node);
    });//开启一个线程来执行rclcpp::spin
    spin_thread.detach();//分离线程，使其在后台运行
    app.exec();
    return 0;
}