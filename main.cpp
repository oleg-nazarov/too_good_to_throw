#include <uwebsockets/App.h>

#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace {

const std::string COMMAND = "command";
const std::string PRIVATE_MSG = "private_msg";
const std::string PUBLIC_MSG = "public_msg";
const std::string MESSAGE = "message";
const std::string USER_ID = "user_id";
const std::string USER_ID_TO = "user_id_to";
const std::string USER_ID_FROM = "user_id_from";
const std::string SET_NAME = "set_name";
const std::string NEW_NAME = "new_name";
const std::string STATUS = "status";
const std::string ONLINE = "online";
const std::string NAME = "name";
const std::string PUBLIC_ALL = "public_all";

struct UserData {
    int user_id;
    std::string name;

    static int latest_id;
};
int UserData::latest_id = 0;

using json = nlohmann::json;
typedef uWS::WebSocket<false, true, UserData> UWEBSOCK;

std::string GetDefaultUserName(int user_id) {
    return "user_" + std::to_string(user_id);
}

// все подключенные пользователи
std::map<int, UserData*> all_users;

std::string Status(UserData* data, bool online) {
    json response;
    response[COMMAND] = STATUS;
    response[NAME] = data->name;
    response[USER_ID] = data->user_id;
    response[ONLINE] = online;

    return response.dump();
}

void ProcessPrivateMsg(UWEBSOCK* ws, json parsed) {
    UserData* data = ws->getUserData();

    int user_id_to = parsed[USER_ID_TO];
    std::string user_msg = parsed[MESSAGE];

    json response;
    response[COMMAND] = PRIVATE_MSG;
    response[USER_ID_FROM] = data->user_id;
    response[MESSAGE] = user_msg;

    ws->publish(GetDefaultUserName(user_id_to), response.dump());
}
void ProcessPublicMsg(UWEBSOCK* ws, json parsed) {
    UserData* data = ws->getUserData();

    std::string user_msg = parsed[MESSAGE];

    json response;
    response[COMMAND] = PUBLIC_MSG;
    response[USER_ID_FROM] = data->user_id;
    response[MESSAGE] = user_msg;

    ws->publish(PUBLIC_ALL, response.dump());
}
void ProcessSetName(UWEBSOCK* ws, json parsed) {
    UserData* data = ws->getUserData();
    data->name = parsed[NEW_NAME];
}

void ProcessMessage(UWEBSOCK* ws, std::string_view message) {
    UserData* data = ws->getUserData();

    std::cout << "Message from user ID: " << data->user_id << "; message: " << message << std::endl;

    auto parsed = json::parse(message);

    if (parsed[COMMAND] == PRIVATE_MSG) {
        ProcessPrivateMsg(ws, parsed);
        return;
    }

    if (parsed[COMMAND] == PUBLIC_MSG) {
        ProcessPublicMsg(ws, parsed);
        return;
    }

    if (parsed[COMMAND] == SET_NAME) {
        ProcessSetName(ws, parsed);
        ws->publish(PUBLIC_ALL, Status(data, true));
        return;
    }
}

}  // namespace

int main() {
    // "/*" - запускаем сервер в корневом разделе нашего сайта
    uWS::App()
        .ws<UserData>(
            "/*",
            {
                .idleTimeout = 15 * 60,
                .open = [](auto* ws) {
                    UserData* data = ws->getUserData();
                    int user_id = ++data->latest_id;
                    data->user_id = user_id;
                    data->name = GetDefaultUserName(user_id);

                    // сообщаем новичку об уже подключенных участниках
                    for(auto& entry : all_users) {
                        ws->send(Status(entry.second, true), uWS::OpCode::TEXT);
                    }

                    all_users[data->user_id] = data;

                    std::cout << "New user connected ID: " << data->user_id << std::endl;

                    ws->subscribe(GetDefaultUserName(data->user_id));
                    ws->subscribe(PUBLIC_ALL);
                    
                    ws->publish(PUBLIC_ALL, Status(data, true)); },
                .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
                    // COMMAND = private_msg
                    // Сервер получает текст { "command": "private_msg", "user_id_to": 13, "message": "Привет 13-й"}
                    // Сервер высылает получателю { "command": "private_msg", "user_id_from": 11, "message": "Привет 13-й"}
                    
                    // COMMAND = public_msg
                    // Сервер получает текст { "command": "public_msg", "message": "Привет всем"}

                    // COMMAND = set_name
                    // Сервер получает текст { "command": "set_name", "name": "Петр"}
                    ProcessMessage(ws, message); },
                .close = [](auto* ws, int code, std::string_view message) {
                    UserData* data = ws->getUserData();

                    std::cout << "User disconnected ID: " << data->user_id << std::endl;
                
                    // TODO: doesn't work, why?
                    ws->publish(PUBLIC_ALL, Status(data, false));

                    all_users.erase(data->user_id); },
            })
        .listen(9001, [](auto* /*listen_socket*/) {
            std::cout << "Server started successfully" << std::endl;
        })
        .run();

    return 0;
}
