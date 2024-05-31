# Use the official Ubuntu base image
FROM ubuntu:latest

# Update the package repository and install valgrind
RUN apt-get update && apt-get install -y valgrind

# Install other necessary packages, e.g., git, build-essential, etc.
RUN apt-get install -y git build-essential

# Create a directory for the web server
WORKDIR /usr/src/app

# Copy the current directory contents into the container at /usr/src/app
COPY . .

# Expose the port the web server runs on
# EXPOSE 5573

# Run the web server
CMD ["bash"]
