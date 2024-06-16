# Define variables
CC = gcc
CFLAGS = -Wall -lm
TARGET = speaker_noise
SRCS = main.c
INSTALL_DIR = /home/$(USER)/.local/bin
SERVICE_DIR = /home/$(USER)/.config/systemd/user/
SERVICE_FILE = speaker_noise.service

# Default target
all: $(TARGET)

# Compile the program
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) -lportaudio

# Install target
install: $(TARGET) $(SERVICE_FILE)
	 install -d $(INSTALL_DIR)
	 install -m 755 $(TARGET) $(INSTALL_DIR)
	 install -d $(SERVICE_DIR)
	 install -m 755 $(SERVICE_FILE) $(SERVICE_DIR)
	 sed -i "s|path|$(INSTALL_DIR)/$(TARGET)|g" $(SERVICE_DIR)/$(SERVICE_FILE)
	 systemctl --user daemon-reload
	 systemctl --user enable $(TARGET)
	 systemctl --user start $(TARGET)

# Clean target
clean:
	rm -f $(TARGET)

# Uninstall target
uninstall:
	systemctl stop $(TARGET)
	systemctl disable $(TARGET)
	rm -f $(INSTALL_DIR)/$(TARGET)
	rm -f $(SERVICE_DIR)/$(SERVICE_FILE)
	systemctl daemon-reload

.PHONY: all clean install uninstall

