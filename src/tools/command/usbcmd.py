#!/usr/bin/env python3
"""
Interactive CLI for DHT20 Humidity Sensor Project
Inspired by professional devcomm systems but simplified for educational use
"""
import serial
import time
import cmd
import sys
import glob
import argparse
import random
from quotes import QUIT_QUOTES
from serial.tools import list_ports

def print_quit_quote():
    """Print a random inspirational quote"""
    quote = random.choice(QUIT_QUOTES)
    print(f"\n{quote}\n")

class PicoShell(cmd.Cmd):
    """
    Interactive shell for sending commands to Pico"""

    intro = """
    ╔═══════════════════════════════════════════╗
    ║   DHT20 Sensor Project - Command Shell    ║
    ╚═══════════════════════════════════════════╝

    Type 'help' for available local commands
    Type any other command to send to Pico
    """

    def __init__(self, port, baudrate=115200, verbose=False):
        super().__init__()
        self.verbose = verbose
        self.port_name = port

        try:
            self.ser = serial.Serial(port, baudrate, timeout=1)
            time.sleep(2)   # Wait for connection
            print(f"Connected to {port} at {baudrate} baud\n")
        except serial.SerialException as e:
            print(f"Failed to connect to {port}: {e}")
            sys.exit(1)

    def send_command(self, command):
        """Send command to Pico and return response"""
        if self.verbose:
            print(f"[TX] {command}")
        
        try:
            # Send command
            self.ser.write(f"{command}\n".encode())

            time.sleep(0.1)
            
            # Read response with timeout
            response_lines = []
            start_time = time.time()
            last_data_time = time.time()
            
            while True:
                # Overall timeout (don't wait forever)
                if time.time() - start_time > 5.0:
                    break
                
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                    if line:
                        response_lines.append(line)
                        if self.verbose:
                            print(f"[RX] {line}")
                        last_data_time = time.time()
                else:
                    # If we got at least one line and haven't seen data for 100ms, we're done
                    if response_lines and (time.time() - last_data_time > 0.1):
                        break
                    time.sleep(0.01)
            
            return response_lines
        
        except serial.SerialException as e:
            print(f"✗ Serial error: {e}")
            return []
    
    def print_response(self, lines):
        """Pretty print response from Pico"""
        for line in lines:
            print(f"  {line}")

    def default(self, line):
        """Handle all commands by forwarding to Pico"""
        line = line.strip()

        if not line:
            return

        if line.lower() in ['exit', 'quit', 'q']:
            return self.do_exit(None)

        # Send to Pico
        response = self.send_command(line)

        if response:
            self.print_response(response)
        else:
            print("NO RESPONSE")

    def do_exit(self, arg):
        """Exit the CLI"""
        print_quit_quote()
        print("\nClosing connection... Goodbye!")
        self.ser.close()
        return True

    def do_help(self, arg):
        """Show help from Pico (overrides built-in help)"""
        # Forward 'help' to Pico instead of showing local help
        response = self.send_command("help")
        if response:
            self.print_response(response)
        else:
            print("  (no response from Pico)")

def main():
    parser = argparse.ArgumentParser(
        description="Interactive CLI for DHT20 Sensor Project",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                           # Auto-detect Pico port
  %(prog)s /dev/tty.usbmodem14201    # Specify port
  %(prog)s -p COM5                   # Windows
  %(prog)s -v                        # Verbose mode
  %(prog)s -c "mock_temp 30"         # Run single command and exit
        """
    )
    
    parser.add_argument(
        'port',
        nargs='?',
        help='Serial port (REQUIRED)'
    )

    parser.add_argument(
        '-b', '--baudrate',
        type=int,
        default=115200,
        help='Baud rate (default: 115200)'
    )
    
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Enable verbose mode (show TX/RX)'
    )
    
    parser.add_argument(
        '-c', '--command',
        help='Execute single command and exit'
    )
    
    parser.add_argument(
        '-l', '--list',
        action='store_true',
        help='List available serial ports'
    )
    
    args = parser.parse_args()
    
    # List ports and exit
    if args.list:
        print("Available serial ports:")
        for port in list_ports.comports():
            print(f"  {port.device} - {port.description}")
        return
    
    # Determine port
    port = args.port
    
    # Create shell
    shell = PicoShell(port, args.baudrate, args.verbose)
    
    # Interactive mode
    try:
        shell.cmdloop()
    except KeyboardInterrupt:
        print("\n\nInterrupted by user")
        shell.do_exit(None)


if __name__ == "__main__":
    main()
