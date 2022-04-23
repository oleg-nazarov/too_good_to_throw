# WebChat

## Prerequisitions:

- CMake 3.22
- vcpkg
- uWebsockets
- nlohman-json

## Hot to use

1. Open a new tab in browser
2. Type in the console the following commands:
    ```
    webSocket= new WebSocket("ws://localhost:9001/");
    webSocket.onmessage= ({data}) => console.log(data);
    ```
3. Add as much tabs as you want and do the 2nd step for them
4. Use one of the following commands:
    - `{ "private_msg": "*", "user_id_to": *, "message": "*" }`
    - `{ "public_msg": "*", "message": "*" }`
    - `{ "set_name": "name": "*" }`
    
    and see the results.

    For example:
    ```
    webSocket.send(JSON.stringify({"command":"private_msg", "user_id_to":11, "message":"Hello 11th!!!"}));
    ```
5. Feel free to play!
