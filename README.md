# Automatic Data Logger Spreadsheets with ESP-32

## Overview

This project enables automatic data logging using an ESP-32 microcontroller. The ESP-32 is connected to the internet, allowing it to send data to Google Sheets through Google Apps Script tokens. This seamless integration ensures real-time data updates on Google Sheets, making it a convenient solution for remote data monitoring and logging.

## Features

- **ESP-32 Connectivity:** The ESP-32 microcontroller is employed for its reliable connectivity options, ensuring a stable connection to the internet.

- **Google Apps Script Token:** The project utilizes Google Apps Script tokens for secure communication between the ESP-32 and Google Sheets. This token-based approach ensures data integrity and privacy.

- **Automated Data Logging:** The ESP-32 autonomously sends data to the designated Google Sheets spreadsheet, enabling automated data logging without manual intervention.

## Getting Started

To begin using this project, follow these steps:

1. **Hardware Setup:** Connect the ESP-32 to your data source or sensor. Ensure that the ESP-32 is configured with the necessary credentials for internet connectivity.

2. **Google Apps Script Configuration:** Set up a Google Apps Script project to generate the required token for communication between the ESP-32 and Google Sheets. Detailed instructions can be found in the project documentation.

3. **Upload Code to ESP-32:** Upload the provided code to your ESP-32 device. This code establishes the connection to the internet, generates the necessary token, and facilitates the data transfer to Google Sheets.

4. **Monitor Data on Google Sheets:** Access your designated Google Sheets spreadsheet to view real-time updates of the logged data. The automated process ensures a streamlined and efficient data logging experience.

## Project Structure

- **/src:** Contains the source code for the ESP-32, including the configuration for internet connectivity and data transfer.

- **/docs:** Project documentation, including setup instructions and usage guidelines.

## Contributing

If you'd like to contribute to this project, please follow our [contribution guidelines](CONTRIBUTING.md). We welcome feedback, bug reports, and feature requests.

## License

This project is licensed under the [MIT License](LICENSE.md).

Feel free to reach out if you have any questions or need assistance with the project.
