import sys
import serial
import threading
import collections
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QHBoxLayout, QWidget, QLabel
from PyQt5.QtGui import QPalette, QColor, QPixmap
from PyQt5.QtCore import Qt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

class MplCanvas(FigureCanvas):
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig, self.axes = plt.subplots(figsize=(width, height), dpi=dpi)
        super(MplCanvas, self).__init__(self.fig)
        self.fig.tight_layout()
        self.fig.patch.set_facecolor('black')
        self.axes.set_facecolor('black')
        self.axes.tick_params(colors='white')
        self.axes.xaxis.label.set_color('white')
        self.axes.yaxis.label.set_color('white')

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("PyQt and Matplotlib")
        self.setGeometry(100, 100, 800, 600)

        # Set the background color of the main window to black
        palette = self.palette()
        palette.setColor(QPalette.Window, QColor('black'))
        self.setPalette(palette)

        # Create a widget to hold the plot
        central_widget = QWidget()
        central_widget.setAutoFillBackground(True)
        palette = central_widget.palette()
        palette.setColor(QPalette.Window, QColor('black'))
        central_widget.setPalette(palette)
        self.setCentralWidget(central_widget)

        # Create a vertical layout
        main_layout = QVBoxLayout(central_widget)

        # Create a horizontal layout for the logo and taskbar
        logo_layout = QVBoxLayout()
        main_layout.addLayout(logo_layout)

        # Load and add the logo
        logo_label = QLabel()
        pixmap = QPixmap(r"C:\Users\kumar\Downloads\GUI\GUI\udupi logo-modified.png")  # Change to your logo's path
        scaled_pixmap = pixmap.scaled(100, 100, Qt.KeepAspectRatio, Qt.SmoothTransformation)  # Adjust the size here
        logo_label.setPixmap(scaled_pixmap)
        logo_label.setAlignment(Qt.AlignLeft | Qt.AlignTop)
        logo_layout.addWidget(logo_label)

        # Create and add the orange taskbar
        taskbar = QLabel()
        taskbar.setFixedHeight(5)  # Adjust the height as needed
        taskbar_palette = taskbar.palette()
        taskbar.setAlignment(Qt.AlignTop)
        taskbar_palette.setColor(QPalette.Window, QColor('red'))
        taskbar.setAutoFillBackground(True)
        taskbar.setPalette(taskbar_palette)
        logo_layout.addWidget(taskbar)

        # Create a vertical layout for the plots and data display
        plots_data_layout = QHBoxLayout()
        main_layout.addLayout(plots_data_layout)

        # Create a vertical layout for the plots
        plots_layout = QVBoxLayout()
        plots_data_layout.addLayout(plots_layout)

        # Create two Matplotlib canvases
        self.accel_canvas = MplCanvas(self, width=5, height=4, dpi=100)
        self.gyro_canvas = MplCanvas(self, width=5, height=4, dpi=100)

        # Add the canvases to the layout
        plots_layout.addWidget(self.accel_canvas)
        plots_layout.addWidget(self.gyro_canvas)

        # Create a vertical layout for the data display box
        data_layout = QVBoxLayout()
        plots_data_layout.addLayout(data_layout)

        # Add a heading for the data box
        data_heading = QLabel("Other Data")
        data_heading.setStyleSheet("color: white; font-size: 16px; font-weight: bold;")
        data_layout.addWidget(data_heading, alignment=Qt.AlignTop)

        # Create labels for the data
        self.temperature_label = QLabel("Temperature: N/A")
        self.pressure_label = QLabel("Pressure: N/A")
        self.humidity_label = QLabel("Humidity: N/A")
        self.altitude_label = QLabel("Altitude: N/A")

        # Set the style for the labels
        for label in [self.temperature_label, self.pressure_label, self.humidity_label, self.altitude_label]:
            label.setStyleSheet("color: white; font-size: 14px;")
            data_layout.addWidget(label, alignment=Qt.AlignTop)

        # Data buffers
        self.ax_data = collections.deque(maxlen=100)
        self.ay_data = collections.deque(maxlen=100)
        self.az_data = collections.deque(maxlen=100)
        self.gx_data = collections.deque(maxlen=100)
        self.gy_data = collections.deque(maxlen=100)
        self.gz_data = collections.deque(maxlen=100)

        # Start the thread to read serial data
        self.serial_thread = threading.Thread(target=self.update_data, daemon=True)
        self.serial_thread.start()

        # Setup the plot
        self.setup_plot()

    def setup_plot(self):
        # Accelerometer plot setup
        self.accel_lines = [
            self.accel_canvas.axes.plot([], [], label='X', color='red')[0],
            self.accel_canvas.axes.plot([], [], label='Y', color='green')[0],
            self.accel_canvas.axes.plot([], [], label='Z', color='blue')[0]
        ]
        self.accel_canvas.axes.legend(loc='upper right', facecolor='black', edgecolor='white', labelcolor='white')

        # Gyroscope plot setup
        self.gyro_lines = [
            self.gyro_canvas.axes.plot([], [], label='X', color='violet')[0],
            self.gyro_canvas.axes.plot([], [], label='Y', color='yellow')[0],
            self.gyro_canvas.axes.plot([], [], label='Z', color='blue')[0]
        ]
        self.gyro_canvas.axes.legend(loc='upper right', facecolor='black', edgecolor='white', labelcolor='white')

    def update_data(self):
        ser = serial.Serial('COM4', 115200)  # Change to your port and baud rate
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').strip()
                data = line.split(',')
                if len(data) == 10:
                    ax, ay, az, gx, gy, gz, temp, pressure, humidity, altitude = map(float, data)
                    self.ax_data.append(ax)
                    self.ay_data.append(ay)
                    self.az_data.append(az)
                    self.gx_data.append(gx)
                    self.gy_data.append(gy)
                    self.gz_data.append(gz)
                    self.update_plot()
                    self.update_display(temp, pressure, humidity, altitude)

    def update_plot(self):
        x = range(len(self.ax_data))

        # Update accelerometer plots
        self.accel_lines[0].set_data(x, list(self.ax_data))
        self.accel_lines[1].set_data(x, list(self.ay_data))
        self.accel_lines[2].set_data(x, list(self.az_data))
        self.accel_canvas.axes.relim()
        self.accel_canvas.axes.autoscale_view()
        self.accel_canvas.draw()

        # Update gyroscope plots
        self.gyro_lines[0].set_data(x, list(self.gx_data))
        self.gyro_lines[1].set_data(x, list(self.gy_data))
        self.gyro_lines[2].set_data(x, list(self.gz_data))
        self.gyro_canvas.axes.relim()
        self.gyro_canvas.axes.autoscale_view()
        self.gyro_canvas.draw()

    def update_display(self, temp, pressure, humidity, altitude):
        self.temperature_label.setText(f"Temperature: {temp} °C")
        self.pressure_label.setText(f"Pressure: {pressure} hPa")
        self.humidity_label.setText(f"Humidity: {humidity} %")
        self.altitude_label.setText(f"Altitude: {altitude} m")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
