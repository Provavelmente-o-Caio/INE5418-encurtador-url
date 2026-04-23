use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fs;
use std::sync::{Arc, Mutex};

#[derive(Clone)]
pub struct AppState {
    pub db: Db,
    pub host: String,
}

#[derive(Serialize, Deserialize, Clone)]
pub struct UrlEntry {
    pub url: String,
    pub acess_count: u64,
}

pub type Db = Arc<Mutex<HashMap<String, UrlEntry>>>;

const DB_FILE: &str = "db.json";

pub fn load_database() -> Db {
    let map: HashMap<String, UrlEntry> = match fs::read_to_string(DB_FILE) {
        Ok(content) => serde_json::from_str(&content).unwrap_or_default(),
        Err(_) => HashMap::new(),
    };
    Arc::new(Mutex::new(map))
}

pub fn save_database(map: &HashMap<String, UrlEntry>) {
    let json = serde_json::to_string_pretty(map).unwrap();
    fs::write(DB_FILE, json).unwrap();
}
