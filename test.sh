SERIAL_PORT=$(change.exe port /query | grep -Po -m 1 "\d" | head -1)
echo "Serial port detected as COM${SERIAL_PORT}"