Here’s a `README.md` template for your project. This includes installation steps, prerequisites, usage, and other important information for users who might want to use or contribute to your `MSVC_19.dll` PHP extension.

---

# MSVC_19 PHP Extension

`MSVC_19` is a PHP extension that provides functions for retrieving memory and process information on Windows systems. This extension can be used to monitor system resources, list running processes, and fetch detailed stats on memory and CPU usage per process.

## Features

- **win32_ps_stat_mem()**: Returns memory status information.
- **win32_ps_stat_proc()**: Returns detailed stats for a specific process.
- **win32_ps_list_procs()**: Lists all running processes with associated information.

## Prerequisites

- **PHP 8.3.x** (or compatible version)
- **Windows OS** (This extension is Windows-specific)
- **Visual Studio 2019** for building the extension (required for PHP compatibility)

## Installation

### 1. Download and Compile

1. **Clone the Repository**:
   ```sh
   git clone https://github.com/Bearsampp/win32ps.git
   cd win32ps
   ```

2. **Build the Extension**:
   - Open the project in **Visual Studio 2019** (required for PHP 8.3.x compatibility).
   - Set the configuration to **Release** and **x64**.
   - Build the project to create `MSVC_19.dll`.

3. **Move the DLL**:
   - Copy the generated `MSVC_19.dll` file to your PHP `ext` directory, typically found at:
     ```plaintext
     C:\path\to\php\ext\
     ```

### 2. Update `php.ini`

1. Open `php.ini` in your PHP directory (e.g., `C:\path\to\php\php.ini`).
2. Add the following line to enable the extension:
   ```ini
   extension=MSVC_19.dll
   ```

3. Save and close `php.ini`.

### 3. Verify Installation

Open a Command Prompt and run:
```sh
php -m
```
You should see `MSVC_19` listed among the loaded extensions.

## Usage

Create a PHP file (e.g., `test_win32ps.php`) with the following code to test the extension:

```php
<?php
echo "\nwin32_ps_stat_mem():\n";
print_r(win32_ps_stat_mem());
echo "\nwin32_ps_stat_proc():\n";
print_r(win32_ps_stat_proc());
echo "\nwin32_ps_list_procs():\n";
print_r(win32_ps_list_procs());
?>
```

Run the test file from the command line:
```sh
php path\to\test_win32ps.php
```

## Functions

### `win32_ps_stat_mem()`
Returns memory statistics of the system, including physical and virtual memory usage.

### `win32_ps_stat_proc()`
Returns detailed stats for a specific process. If no process ID is given, it defaults to the current process.

### `win32_ps_list_procs()`
Lists all running processes on the system along with basic details.

## Contributing

Contributions are welcome! Please fork this repository, make your changes, and submit a pull request.

### Bug Reports & Feature Requests

Please use the [GitHub issue tracker](https://github.com/yourusername/MSVC_19/issues) to report bugs or request features.

## License

This project is open-source and available under the [MIT License](LICENSE).

