# WebChat

## Prerequisitions:

- `CMake 3.22`
- with `vcpkg` install `uwebsockets` and `nlohman-json` (see [getting started](https://vcpkg.io/en/getting-started.html))

## Hot to use

1. Run the program and start the server
2. Open a new tab in browser and type in the console the following commands:
    ```
    webSocket= new WebSocket("ws://localhost:9001/");
    webSocket.onmessage= ({data}) => console.log(data);
    ```
3. Add as much tabs as you want and do the 2nd step for them
4. Use one of the following commands:
    - `{ "command": "private_msg", "user_id_to": *, "message": "*" }`
    - `{ "command": "public_msg", "message": "*" }`
    - `{ "command": "set_name", "new_name": "*" }`
    
    and see the results.

    For example:
    ```
    webSocket.send(JSON.stringify({"command":"private_msg", "user_id_to":11, "message":"Hello 11th!!!"}));
    ```
5. Feel free to play!
