use std::collections::HashMap;
use std::fs;

pub struct Config {
    pub host: String,
    pub port: u16,
}

impl Config {
    pub fn load() -> Self {
        let content: String =
            fs::read_to_string("config.txt").expect("Configuraition file not found");

        let map: HashMap<&str, &str> = content
            .lines()
            .filter_map(|line| line.split_once('='))
            .collect();

        Config {
            host: map
                .get("host")
                .expect("Host definition not found")
                .to_string(),
            port: map
                .get("port")
                .expect("Port definition not found")
                .parse()
                .expect("Port definition is not a valid number"),
        }
    }
}
