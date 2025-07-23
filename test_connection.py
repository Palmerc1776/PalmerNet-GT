#!/usr/bin/env python3
"""
Simple connection test for Growtopia server
This script tests if your server is responding to basic connections
"""

import socket
import time
import sys

def test_connection(host, port):
    print(f"🔍 Testing connection to {host}:{port}")
    print("=" * 50)
    
    try:
        # Test basic TCP connection
        print("1. Testing TCP connection...")
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(10)
        
        result = sock.connect_ex((host, port))
        if result == 0:
            print("   ✅ TCP connection successful!")
            
            # Try to send a basic message
            print("2. Testing basic communication...")
            test_message = b"requestedName|test\ntankIDName|test\ntankIDPass|test\n"
            
            try:
                sock.send(test_message)
                print("   ✅ Message sent successfully!")
                
                # Try to receive response
                print("3. Waiting for server response...")
                sock.settimeout(5)
                response = sock.recv(1024)
                
                if response:
                    print(f"   ✅ Server responded with {len(response)} bytes!")
                    print(f"   📋 Response preview: {response[:100]}...")
                else:
                    print("   ⚠️  No response received")
                    
            except socket.timeout:
                print("   ⚠️  Timeout waiting for response")
            except Exception as e:
                print(f"   ❌ Communication error: {e}")
                
        else:
            print(f"   ❌ TCP connection failed (Error: {result})")
            
        sock.close()
        
    except Exception as e:
        print(f"   ❌ Connection test failed: {e}")
    
    print("\n" + "=" * 50)

def main():
    if len(sys.argv) > 1:
        host = sys.argv[1]
    else:
        host = "137.220.62.28"  # Your VPS IP
    
    port = 17091
    
    print("🚀 Growtopia Server Connection Test")
    print(f"Target: {host}:{port}")
    print()
    
    test_connection(host, port)
    
    print("\n💡 Tips:")
    print("   - If TCP connection fails: Check if server is running")
    print("   - If no response: Check server logs for connection attempts")
    print("   - If timeout: Server might not be handling the protocol correctly")

if __name__ == "__main__":
    main()