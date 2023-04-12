use std::io::{BufRead, BufReader};
use std::process::exit;

use anyhow::Result;
use log::info;
use midir::MidiOutput;
use midir::os::unix::VirtualOutput;

fn start() -> Result<()> {
    let midi_out = MidiOutput::new("MIDI Output")?;
    let mut conn_out = midi_out.create_virtual("Virtual MIDI Output").unwrap();

    let serial_port = serialport::new("/dev/cu.wchusbserial1450", 9600)
        .timeout(std::time::Duration::from_millis(10))
        .open()?;
    let mut reader = BufReader::new(serial_port);

    let mut buf = Vec::new();

    info!("Starting");

    loop {
        match reader.read_until(b'\n', &mut buf) {
            Ok(t) => {
                if t == 0 { continue }

                // Convert buffer to string
                let mut line = String::from_utf8_lossy(&buf).to_string();
                line = line.trim().to_string();
                buf.clear();

                println!("{}", line);

                // Only process commands
                if !line.starts_with("/") { continue }

                // Split command arguments
                let args: Vec<&str> = line.split_whitespace().collect();

                // Command /hit <note> <velocity>
                // Usage example: /hit 60 127
                if args[0] == "/hit" {
                    let note = args[1].parse::<u8>()?;
                    // let note = 72; // c5
                    let velocity = args[2].parse::<u8>()?;
                    conn_out.send(&[0x99, note, velocity]).unwrap();

                    // Since it's an instant hit, we need to send a note off
                    // Sleep for 10ms to make sure the note off is sent after the note on
                    // std::thread::sleep(std::time::Duration::from_millis(10));
                    conn_out.send(&[0x89, note, 0]).unwrap();
                }

                // Command /pedal <0 = off / 1 = on>
                // Usage example: /pedal 0
                if args[0] == "/pedal" {
                    let pedal = args[1].parse::<u8>()? * 127;
                    conn_out.send(&[0xB0, 64, pedal]).unwrap();
                }
            },
            Err(ref e) if e.kind() == std::io::ErrorKind::TimedOut => (),
            Err(e) => {
                if e.kind() == std::io::ErrorKind::BrokenPipe {
                    println!("Connection closed.");
                    exit(0);
                }
            },
        }
    }
}

fn main() {
    pretty_env_logger::init();
    start().expect("Error: Start failed.");
}
