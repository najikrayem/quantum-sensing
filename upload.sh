echo "Starting the program loading process..."

# Run a cmd command to make the board enter bootloader mode programmatically
echo "Putting the board into bootloader mode..."
bootloader_error=$(cmd.exe /c "Mode Com4: Baud=1200 Parity=N Data=8 Stop=1" 2>&1 > /dev/null)
# if [ $? -ne 0 ]; then
#     if echo "$bootloader_error" | grep -q "The specified options are not supported by this serial device"; then
#         echo "Bootloader mode already active. Continuing..."
#     else
#         echo "Failed to put the board into bootloader mode. Error: $bootloader_error"
#         exit 1
#     fi
# fi
sleep 1

# Mount the board's drive
echo "Mounting the board's drive (you may be prompted for your password)..."
mount_error=$(sudo mount -t drvfs E: /mnt/e 2>&1 > /dev/null)
if [ $? -ne 0 ]; then
    echo "Failed to mount the board's drive. Error: $mount_error"
    exit 1
fi

# Copy the UF2 file to the board
echo "Copying the UF2 file to the board..."
cp build/2D_antenna.uf2 /mnt/e/
if [ $? -ne 0 ]; then
    echo "Failed to copy the UF2 file to the board. Exiting."
    exit 1
fi
sleep 1

# Open a serial connection using PuTTY
echo "Opening a serial connection to the board using PuTTY..."
putty.exe -load "pico" > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Failed to open a serial connection using PuTTY. Exiting."
    exit 1
fi

echo "Program loading process completed successfully!"