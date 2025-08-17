#!/bin/bash

# Quick Start Script for WiFi LED Control Project
# This script provides an overview and quick setup guide

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to print colored output
print_header() {
    echo -e "${CYAN}================================${NC}"
    echo -e "${CYAN}  WiFi LED Control Project${NC}"
    echo -e "${CYAN}================================${NC}"
}

print_section() {
    echo -e "${BLUE}$1${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check project structure
check_project_structure() {
    print_section "Checking project structure..."
    
    local missing_files=()
    
    # Check for required files
    [[ -f "README.md" ]] || missing_files+=("README.md")
    [[ -f "SETUP_GUIDE.md" ]] || missing_files+=("SETUP_GUIDE.md")
    [[ -f "src/wifi_led_control.ino" ]] || missing_files+=("src/wifi_led_control.ino")
    [[ -f "src/simple_test.ino" ]] || missing_files+=("src/simple_test.ino")
    [[ -f "config/wifi_config.h" ]] || missing_files+=("config/wifi_config.h")
    [[ -f "docs/wiring_diagram.md" ]] || missing_files+=("docs/wiring_diagram.md")
    [[ -f "docs/troubleshooting.md" ]] || missing_files+=("docs/troubleshooting.md")
    [[ -f "tools/upload_script.sh" ]] || missing_files+=("tools/upload_script.sh")
    
    if [[ ${#missing_files[@]} -eq 0 ]]; then
        print_success "All project files are present"
    else
        print_error "Missing files: ${missing_files[*]}"
        return 1
    fi
}

# Function to check system requirements
check_system_requirements() {
    print_section "Checking system requirements..."
    
    local missing_tools=()
    
    # Check for required tools
    command_exists "arduino-cli" || missing_tools+=("arduino-cli")
    command_exists "git" || missing_tools+=("git")
    
    if [[ ${#missing_tools[@]} -eq 0 ]]; then
        print_success "All required tools are installed"
    else
        print_warning "Missing tools: ${missing_tools[*]}"
        echo ""
        echo "To install missing tools:"
        if [[ " ${missing_tools[@]} " =~ " arduino-cli " ]]; then
            echo "  arduino-cli:"
            echo "    macOS: brew install arduino-cli"
            echo "    Linux: curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh"
            echo "    Windows: Download from https://github.com/arduino/arduino-cli/releases"
        fi
        if [[ " ${missing_tools[@]} " =~ " git " ]]; then
            echo "  git:"
            echo "    macOS: brew install git"
            echo "    Linux: sudo apt-get install git"
            echo "    Windows: Download from https://git-scm.com/"
        fi
        echo ""
    fi
}

# Function to show project overview
show_project_overview() {
    print_section "Project Overview"
    
    echo "This project allows you to control an LED using an ESP8266 WiFi module."
    echo "When the WiFi module connects to the internet, it blinks an LED and"
    echo "provides web-based control through a browser interface."
    echo ""
    
    echo "Key Features:"
    echo "  • WiFi connection with LED indicator"
    echo "  • Web-based LED control interface"
    echo "  • Serial monitoring and debugging"
    echo "  • Optional relay module support"
    echo "  • Comprehensive documentation"
    echo ""
    
    echo "Hardware Requirements:"
    echo "  • ESP8266 WiFi module (NodeMCU, Wemos D1 Mini, etc.)"
    echo "  • Breadboard"
    echo "  • LED (any color)"
    echo "  • 220Ω resistor"
    echo "  • Jumper wires"
    echo "  • USB cable"
    echo ""
}

# Function to show quick setup steps
show_quick_setup() {
    print_section "Quick Setup Steps"
    
    echo "1. Hardware Setup:"
    echo "   • Connect ESP8266 to breadboard"
    echo "   • Connect LED to GPIO2 with 220Ω resistor"
    echo "   • Connect power and ground"
    echo ""
    
    echo "2. Software Setup:"
    echo "   • Install Arduino IDE"
    echo "   • Add ESP8266 board support"
    echo "   • Update WiFi credentials in config/wifi_config.h"
    echo ""
    
    echo "3. Upload Code:"
    echo "   • Start with src/simple_test.ino"
    echo "   • Then upload src/wifi_led_control.ino"
    echo "   • Monitor Serial output (115200 baud)"
    echo ""
    
    echo "4. Test:"
    echo "   • LED should blink when WiFi connects"
    echo "   • Access web interface at ESP8266's IP address"
    echo "   • Test LED control buttons"
    echo ""
}

# Function to show available commands
show_available_commands() {
    print_section "Available Commands"
    
    echo "Project Management:"
    echo "  ./quick_start.sh              # Show this overview"
    echo "  ./tools/upload_script.sh      # Upload code to ESP8266"
    echo "  ./tools/upload_script.sh -l   # List available ports"
    echo "  ./tools/upload_script.sh -c   # Compile only"
    echo ""
    
    echo "Documentation:"
    echo "  cat README.md                  # Project overview"
    echo "  cat SETUP_GUIDE.md            # Detailed setup guide"
    echo "  cat docs/wiring_diagram.md    # Hardware connections"
    echo "  cat docs/troubleshooting.md   # Common issues"
    echo ""
    
    echo "Code Files:"
    echo "  src/wifi_led_control.ino      # Main program"
    echo "  src/simple_test.ino           # Test program"
    echo "  config/wifi_config.h          # WiFi credentials"
    echo ""
}

# Function to show next steps
show_next_steps() {
    print_section "Next Steps"
    
    echo "1. Read the documentation:"
    echo "   • README.md - Project overview"
    echo "   • SETUP_GUIDE.md - Detailed instructions"
    echo "   • docs/wiring_diagram.md - Hardware setup"
    echo ""
    
    echo "2. Set up your hardware:"
    echo "   • Follow the wiring diagram"
    echo "   • Double-check all connections"
    echo "   • Ensure proper power supply"
    echo ""
    
    echo "3. Configure software:"
    echo "   • Update WiFi credentials"
    echo "   • Install Arduino IDE"
    echo "   • Add ESP8266 board support"
    echo ""
    
    echo "4. Upload and test:"
    echo "   • Start with simple_test.ino"
    echo "   • Then try wifi_led_control.ino"
    echo "   • Monitor Serial output"
    echo ""
    
    echo "5. Troubleshoot if needed:"
    echo "   • Check docs/troubleshooting.md"
    echo "   • Verify all connections"
    echo "   • Test with different components"
    echo ""
}

# Main function
main() {
    print_header
    echo ""
    
    # Check project structure
    if ! check_project_structure; then
        print_error "Project structure check failed"
        exit 1
    fi
    
    # Check system requirements
    check_system_requirements
    echo ""
    
    # Show project overview
    show_project_overview
    
    # Show quick setup
    show_quick_setup
    
    # Show available commands
    show_available_commands
    
    # Show next steps
    show_next_steps
    
    print_section "Getting Started"
    echo "For detailed instructions, see SETUP_GUIDE.md"
    echo "For troubleshooting, see docs/troubleshooting.md"
    echo ""
    echo "Happy coding! 🚀"
}

# Run main function
main "$@" 