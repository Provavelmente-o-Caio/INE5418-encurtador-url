use std::collections::HashMap;
use std::fs;

pub struct Config {
    pub host: String,
}

impl Config {
    pub fn load() -> Self {
        let content = fs::read_to_string("config.txt").expect("Configuration file not found");

        let map: HashMap<&str, &str> = content
            .lines()
            .filter_map(|line| line.split_once('='))
            .collect();

        Config {
            host: map
                .get("host")
                .expect("Host definition not found")
                .to_string(),
        }
    }
}
