# RandomUnsplash Desktop App

RandomUnsplash is a desktop application written in C++ with Qt that allows you to download a random image from a given category from Unsplash.

## Development

RandomUnsplash is developed using C++ and the Qt framework. It utilizes Qt's network functionality to download images from Unsplash. The main window of the application provides a text input field where you can enter the category of images you want to download. Upon clicking the "Download" button, the application fetches a random image from Unsplash in the specified category and saves it to your local system. You also have the option to choose whether to open the downloaded image automatically after it's saved.

## How to Run Locally

To run RandomUnsplash locally, follow these steps:

1. **Clone the Repository**: Clone the RandomUnsplash repository from GitHub to your local machine.

   ```bash
   git clone https://github.com/lukasklockenhoff/randomunsplash.git
   ```

2. **Build the Application**: Build the application using a C++ compiler (e.g., g++) along with the Qt library. Make sure you have Qt installed on your system.

   ```bash
   cd randomunsplash
   ./build/Qt_6_7_0_for_macOS-Debug/http.app/Contents/MacOS/http
   ```

3. **Run the Application**: Execute the compiled executable to launch the RandomUnsplash application.

   ```bash
   ./randomunsplash
   ```

4. **Usage**: In the application window, enter the category of images you want to download in the provided text field. Click the "Download" button to initiate the download process. The downloaded image will be saved to your local system, and if the option is enabled, it will be opened automatically.

## Contributors

- Lukas Klockenhoff

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
