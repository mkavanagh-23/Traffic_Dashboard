# Traffic Dashboard
#### A real-time traffic dashboard displaying traffic events throughout NYS, Ontario and Montreal.

### Dependencies
This project uses several external libraries:
- [cURL](https://github.com/curl/curl)
- [JsonCpp](https://github.com/open-source-parsers/jsoncpp)
- [RapidXML](https://rapidxml.sourceforge.net/)
- [Gumbo Parser](https://github.com/google/gumbo-parser)
- [Poco](https://github.com/pocoproject/poco)
The CMake build script will automatically download missing dependencies, you may need to install curl or ssl libraries on your system first.

### Build Instructions
The build script currently works on Linux with experimental functionality for both macOS and Windows.
1. Ensure git, c++ build tools, and CMake are installed
2. Clone this repo
3. (Optional) to clean, run `clean.sh`
4. Run the build script
```
./build.sh [release|debug]
```

### Usage
This program requires you to provide your own API key for NYSDOT/511NY. Be sure to run this program with the env variable 'NYSDOT_API_KEY' properly set.

Once running, the program will spin up a web server at the user's specified port where it will listen for API requests.

## TODO:
Develop a web frontend in HTML/CSS/JS to call and interact with data from the C++ http server.
