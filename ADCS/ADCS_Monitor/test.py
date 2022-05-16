import PySimpleGUI as sg
from time import sleep
import serial

sg.theme('Dark Grey 13')
ser = serial.Serial('COM3', 230400, timeout=1)

h_scale = 150       # pixel per unit horizontal length
v_scale = 100       # pixel per unit vertical length

text_font = "Arial 20"
health_text_font = "Arial 14"
health_canvas_size = (50, 20)
gps_text_font = "Arial 14"
serial_text_font = "Arial 12"

# elements
status_text = sg.Text("Online", text_color="green", font=text_font)
temperature_text = sg.Text("temperature", text_color="green", font=text_font)
time_text = sg.Text("Time", text_color="green", font=text_font)
imu_accel_text = sg.Text("IMU Accelerometer", text_color="blue", font=text_font)
imu_gyro_text = sg.Text("IMU GyroScope", text_color="blue", font=text_font)
imu_mag_text = sg.Text("IMU Magnetometer", text_color="blue", font=text_font)
mgn_text = sg.Text("MGN Magnetometer", text_color="blue", font=text_font)

health_gps_c = sg.Canvas(background_color="gray", size=health_canvas_size)
health_imu_c = sg.Canvas(background_color="gray", size=health_canvas_size)
health_mgn_c = sg.Canvas(background_color="gray", size=health_canvas_size)
health_tmp_c = sg.Canvas(background_color="gray", size=health_canvas_size)
health_layout = [[
                sg.Column([[sg.Text("GPS", font=health_text_font)],
                           [health_gps_c]], element_justification='c'),
                sg.Column([[sg.Text("IMU", font=health_text_font)],
                           [health_imu_c]], element_justification='c'),
                sg.Column([[sg.Text("MGN", font=health_text_font)],
                           [health_mgn_c]], element_justification='c'),
                sg.Column([[sg.Text("TMP", font=health_text_font)],
                           [health_tmp_c]], element_justification='c')
                ]]

gps_fix_text = sg.Text("GPS Fix", text_color="yellow", font=gps_text_font)
gps_time_text = sg.Text("GPS Time", text_color="yellow", font=gps_text_font)
gps_lon_text = sg.Text("Longitude", text_color="yellow", font=gps_text_font)
gps_lat_text = sg.Text("Latitude", text_color="yellow", font=gps_text_font)
gps_alt_text = sg.Text("Altitude", text_color="yellow", font=gps_text_font)
gps_satn_text = sg.Text("Sat number", text_color="yellow", font=gps_text_font)
gps_dop_text = sg.Text("DOP", text_color="yellow", font=gps_text_font)
gps_layout = [
    [gps_fix_text],
    [gps_time_text],
    [gps_lon_text],
    [gps_lat_text],
    [gps_alt_text],
    [gps_satn_text],
    [gps_dop_text]
    ]

serial_text = sg.Multiline(size=(8 * h_scale, 3 * v_scale), font=serial_text_font, text_color="cyan",
                           autoscroll=True)


# frames
status_frame = sg.Frame("Status", [[status_text]], size=(2 * h_scale, 1 * v_scale), element_justification='c' )
temperature_frame = sg.Frame("Temperature", [[temperature_text]], size=(3 * h_scale, 1 * v_scale), element_justification='c')
time_frame = sg.Frame("Time", [[time_text]], size=(3 * h_scale, 1 * v_scale), element_justification='c')
imu_frame = sg.Frame("IMU", [[imu_accel_text],
                             [imu_gyro_text],
                             [imu_mag_text]], size=(5 * h_scale, 2 * v_scale), element_justification='c')
mgn_frame = sg.Frame("MGN", [[mgn_text]], size=(5 * h_scale, 1 * v_scale), element_justification='c')
health_frame = sg.Frame("Health Check", health_layout, size=(5 * h_scale, 1 * v_scale), element_justification='c')
gps_frame = sg.Frame("GPS", gps_layout, size=(3 * h_scale, 3 * v_scale))
serial_frame = sg.Frame("Serial Data", [[serial_text]], size=(8 * h_scale, 3 * v_scale))
check_box = sg.Checkbox("Analyze")

# layout
layout = [[status_frame, temperature_frame, time_frame],
          [sg.Column(layout=[[imu_frame], [mgn_frame], [health_frame]]), gps_frame],
          [serial_frame],
          [check_box]
          ]

window = sg.Window('ADCS Monitor', layout, finalize=True)

# flags
first_init = True
first_update = True

if __name__ == '__main__':
    try:
        window.read(timeout=0)
        while True:
            line = ser.readline()
            # skip some garbage characters
            if line.find(b'\xff') != -1:
                continue

            line = line.decode("utf-8")

            # formatting logic
            # PFC
            if line.startswith("PFC"):
                status_text.update(value=line[:-1], text_color="Red")
                serial_text.update(autoscroll=False)
                line = ""
            # Health
            elif line.find("HEALTH") != -1:
                if line.find("IMU") != -1 and line.find("Broken") != -1:
                    health_imu_c.update(background_color="red")
                elif line.find("MGN") != -1 and line.find("Broken") != -1:
                    health_mgn_c.update(background_color="red")
                elif line.find("TMP") != -1 and line.find("Broken") != -1:
                    health_tmp_c.update(background_color="red")
                elif line.find("GPS") != -1 and line.find("Broken") != -1:
                    health_gps_c.update(background_color="red")
            # IMU
            elif line.find("DIAG - IMU") != -1:
                health_imu_c.update(background_color="green")
                if line.find("A:") != -1 :
                    imu_accel_text.update(value=line[line.find("A:"): -1])
                elif line.find("G:") != -1:
                    imu_gyro_text.update(value=line[line.find("G:"): -1])
                elif line.find("M:") != -1:
                    imu_mag_text.update(value=line[line.find("M:"): -1])
            # MGN
            elif line.find("DIAG - MGN") != -1:
                health_mgn_c.update(background_color="green")
                if line.find("X:") != -1 :
                    mgn_text.update(value=line[line.find("X:"): -1])
            # TMP
            elif line.find("DIAG - TMP") != -1:
                health_tmp_c.update(background_color="green")
                if line.find("Temp") != -1:
                    temperature_text.update(value=line[line.find("Temp"): -1])
            # GPS
            elif line.find("GPS") != -1:
                if line.find("fix") != -1:
                    gps_fix_text.update(value=line[line.find("fix"): line.find("lat")-1])
                    gps_lat_text.update(value=line[line.find("lat"): line.find("long") - 1])
                    gps_lon_text.update(value=line[line.find("long"): line.find("alt") - 1])
                    gps_alt_text.update(value=line[line.find("alt"): -1])
                elif line.find("utc") != -1:
                    gps_time_text.update(value=line[line.find("utc"): line.find("satnum")-1])
                    gps_satn_text.update(value=line[line.find("satnum"): line.find("DOP") - 1])
                    gps_dop_text.update(value=line[line.find("DOP"): -1])
            # Time
            elif line.find("Time Keeping") != -1:
                if line.find("20") != -1:
                    time_text.update(value=line[line.find("20"): -1])

            # update the screen
            if check_box.get():
                serial_text.update(autoscroll=False)
            else:
                serial_text.update(autoscroll=True)
                serial_text.write(line)

            window.read(timeout=0)
    finally:
        window.close()
        print("--- Closing port ---")
        ser.close()