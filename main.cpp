#include <uwebsockets/App.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace {

const std::string COMMAND = "command";
const std::string PRIVATE_MSG = "private_msg";
const std::string PUBLIC_MSG = "public_msg";
const std::string MESSAGE = "message";
const std::string USER_ID_TO = "user_id_to";
const std::string USER_ID_FROM = "user_id_from";
const std::string SET_NAME = "set_name";
const std::string NEW_NAME = "new_name";
const std::string UNNAMED = "NONAME";

struct UserData {
    int user_id;
    std::string name;

    static int latest_id;
};
int UserData::latest_id = 0;

} // namespace

int main() {
    using json = nlohmann::json;

    // "/*" - запускаем сервер в корневом разделе нашего сайта
    uWS::App()
        .ws<UserData>(
            "/*",
            {
                .idleTimeout = 15 * 60,
                .open = [](auto* ws) {
                    UserData* data = ws->getUserData();
                    data->user_id = ++data->latest_id;
                    data->name = UNNAMED + '_' + std::to_string(data->user_id);

                    std::cout << "New user connected ID: " << data->user_id << std::endl;

                    ws->subscribe("userN" + std::to_string(data->user_id));
                    ws->subscribe("public_all");
                },
                .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
                    UserData* data = ws->getUserData();

                    std::cout << "Message from user ID: " << data->user_id << "; message: " << message << std::endl;

                    // COMMAND = private_msg
                    // Сервер получает текст { "command": "private_msg", "user_id_to": 13, "message": "Привет 13-й"}
                    // Сервер высылает получателю { "command": "private_msg", "user_id_from": 11, "message": "Привет 13-й"}
                    
                    // COMMAND = public_msg
                    // Сервер получает текст { "command": "public_msg", "message": "Привет всем"}

                    // COMMAND = set_name
                    // Сервер получает текст { "command": "set_name", "name": "Петр"}
                    
                    auto parsed = json::parse(message);
                    
                    if (parsed[COMMAND] == PRIVATE_MSG) {
                        int user_id_to = parsed[USER_ID_TO];
                        std::string user_msg = parsed[MESSAGE];

                        json response;
                        response[COMMAND] = PRIVATE_MSG;
                        response[USER_ID_FROM] = data->user_id;
                        response[MESSAGE] = user_msg;

                        ws->publish("userN" + std::to_string(user_id_to), response.dump());

                        return;
                    }

                    if (parsed[COMMAND] == PUBLIC_MSG) {
                        std::string user_msg = parsed[MESSAGE];

                        json response;
                        response[COMMAND] = PUBLIC_MSG;
                        response[USER_ID_FROM] = data->user_id;
                        response[MESSAGE] = user_msg;

                        ws->publish("public_all", response.dump());

                        return;
                    }

                    if (parsed[COMMAND] == SET_NAME) {
                        data->name = parsed[NEW_NAME];

                        return;
                    }
                },
                .close = [](auto* ws, int code, std::string_view message) {
                    UserData* data = ws->getUserData();

                    std::cout << "User disconnected ID: " << data->user_id << std::endl; },
            })
        .listen(9001, [](auto* /*listen_socket*/) {
            std::cout << "Server started successfully" << std::endl;
        })
        .run();

    return 0;
}
