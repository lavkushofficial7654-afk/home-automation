#!/bin/bash

# ESP8266 Upload Script
# This script helps automate the upload process for ESP8266 projects

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
ARDUINO_CLI="arduino-cli"
SKETCH_PATH="../src/wifi_led_control.ino"
BOARD_TYPE="esp8266:esp8266:nodemcuv2"
PORT=""
BAUD_RATE="115200"

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if arduino-cli is installed
check_arduino_cli() {
    if ! command -v $ARDUINO_CLI &> /dev/null; then
        print_error "arduino-cli is not installed or not in PATH"
        print_status "Please install arduino-cli first:"
        echo "  macOS: brew install arduino-cli"
        echo "  Linux: curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh"
        echo "  Windows: Download from https://github.com/arduino/arduino-cli/releases"
        exit 1
    fi
    print_success "arduino-cli found"
}

# Function to list available ports
list_ports() {
    print_status "Available ports:"
    $ARDUINO_CLI board list
}

# Function to detect ESP8266 port automatically
detect_port() {
    print_status "Detecting ESP8266 port..."
    
    # Get list of ports and look for ESP8266
    PORT_LIST=$($ARDUINO_CLI board list --format json | grep -o '"port":"[^"]*"' | cut -d'"' -f4)
    
    for port in $PORT_LIST; do
        if [[ $port == *"usb"* ]] || [[ $port == *"COM"* ]] || [[ $port == *"tty"* ]]; then
            PORT=$port
            print_success "Detected ESP8266 on port: $PORT"
            return 0
        fi
    done
    
    print_warning "Could not automatically detect ESP8266 port"
    return 1
}

# Function to compile the sketch
compile_sketch() {
    print_status "Compiling sketch..."
    
    if $ARDUINO_CLI compile --fqbn $BOARD_TYPE $SKETCH_PATH; then
        print_success "Compilation successful"
    else
        print_error "Compilation failed"
        exit 1
    fi
}

# Function to upload the sketch
upload_sketch() {
    if [ -z "$PORT" ]; then
        print_error "No port specified. Please specify a port with -p option or let the script detect it."
        list_ports
        exit 1
    fi
    
    print_status "Uploading sketch to ESP8266 on port $PORT..."
    print_warning "Make sure ESP8266 is in upload mode (hold FLASH button while pressing RESET)"
    
    if $ARDUINO_CLI upload --fqbn $BOARD_TYPE --port $PORT $SKETCH_PATH; then
        print_success "Upload successful!"
        print_status "Opening Serial Monitor..."
        sleep 2
        $ARDUINO_CLI monitor --port $PORT --fqbn $BOARD_TYPE --config baudrate=$BAUD_RATE
    else
        print_error "Upload failed"
        print_status "Troubleshooting tips:"
        echo "  1. Check if ESP8266 is in upload mode"
        echo "  2. Verify USB cable connection"
        echo "  3. Check if correct port is selected"
        echo "  4. Try pressing RESET button on ESP8266"
        exit 1
    fi
}

# Function to show help
show_help() {
    echo "ESP8266 Upload Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -p, --port PORT     Specify the port (e.g., /dev/ttyUSB0, COM3)"
    echo "  -b, --board BOARD   Specify board type (default: esp8266:esp8266:nodemcuv2)"
    echo "  -s, --sketch PATH   Specify sketch path (default: ../src/wifi_led_control.ino)"
    echo "  -c, --compile-only  Only compile, don't upload"
    echo "  -l, --list-ports    List available ports"
    echo "  -h, --help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                    # Auto-detect port and upload"
    echo "  $0 -p /dev/ttyUSB0    # Upload to specific port"
    echo "  $0 -c                 # Only compile"
    echo "  $0 -l                 # List available ports"
}

# Parse command line arguments
COMPILE_ONLY=false
LIST_PORTS=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -p|--port)
            PORT="$2"
            shift 2
            ;;
        -b|--board)
            BOARD_TYPE="$2"
            shift 2
            ;;
        -s|--sketch)
            SKETCH_PATH="$2"
            shift 2
            ;;
        -c|--compile-only)
            COMPILE_ONLY=true
            shift
            ;;
        -l|--list-ports)
            LIST_PORTS=true
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Main execution
main() {
    print_status "ESP8266 Upload Script Starting..."
    
    # Check if arduino-cli is available
    check_arduino_cli
    
    # List ports if requested
    if [ "$LIST_PORTS" = true ]; then
        list_ports
        exit 0
    fi
    
    # Compile the sketch
    compile_sketch
    
    # Exit if compile-only mode
    if [ "$COMPILE_ONLY" = true ]; then
        print_success "Compilation completed successfully"
        exit 0
    fi
    
    # Detect port if not specified
    if [ -z "$PORT" ]; then
        if ! detect_port; then
            print_error "Please specify a port using -p option"
            list_ports
            exit 1
        fi
    fi
    
    # Upload the sketch
    upload_sketch
}

# Run main function
main "$@" 