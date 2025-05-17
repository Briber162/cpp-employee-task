#!/bin/bash

# Build both server and client
make httplib_server employee_client

# Kill any existing server instances
pkill -f httplib_server >/dev/null 2>&1 || true
echo "Starting server in background..."

# Start the server in background
./httplib_server &
SERVER_PID=$!

# Wait for server to initialize
echo "Waiting for server to start up..."
sleep 2

# Run the client
echo "Starting client..."
./employee_client

# When client exits, cleanup the server
echo "Client exited. Stopping the server..."
kill $SERVER_PID >/dev/null 2>&1
