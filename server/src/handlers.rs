use axum::{
    Json,
    extract::{Path, State},
    response::IntoResponse,
};
use rand::{RngExt, distr::Alphanumeric};
use serde::{Deserialize, Serialize};
use serde_json::json;

use crate::data::{AppState, UrlEntry, save_database};

#[derive(Deserialize)]
pub struct ShortenRequest {
    pub url: String,
}

#[derive(Serialize)]
struct UrlResponse {
    pub codigo: String,
    pub url_original: String,
    pub acessos: u64,
}

fn generate_code() -> String {
    rand::rng()
        .sample_iter(Alphanumeric)
        .take(6)
        .map(char::from)
        .collect()
}

pub async fn shorten_url(
    State(state): State<AppState>,
    Json(request): Json<ShortenRequest>,
) -> impl IntoResponse {
    let mut map = state.db.lock().unwrap();

    // confere se a url já foi encurtada
    if let Some(entry) = map.get(&request.url) {
        return Json(
            json!({ "codigo": entry.url, "url_curta": format!("{}/r/{}", state.host, entry.url) }),
        );
    }

    // código único
    let code = loop {
        let c = generate_code();
        if !map.contains_key(&c) {
            break c;
        }
    };

    map.insert(
        code.clone(),
        UrlEntry {
            url: request.url,
            acess_count: 0,
        },
    );
    save_database(&map);

    let json_response = json!({
        "codigo": code,
       "url_curta": format!("{}/r/{}", state.host, code),
    });
    Json(json_response)
}

pub async fn delete_url(
    State(state): State<AppState>,
    Path(codigo): Path<String>,
) -> impl IntoResponse {
    let mut map = state.db.lock().unwrap();

    match map.remove(&codigo) {
        Some(_) => {
            save_database(&map);
            Json(json!({ "removido": "true" }))
        }
        None => Json(json!({ "removido": "false" })),
    }
}

pub async fn list_all_urls(State(state): State<AppState>) -> impl IntoResponse {
    let map = state.db.lock().unwrap();

    let json_response: Vec<UrlResponse> = map
        .iter()
        .map(|(codigo, entry)| UrlResponse {
            codigo: codigo.clone(),
            url_original: entry.url.clone(),
            acessos: entry.acess_count,
        })
        .collect();

    Json(json_response)
}

pub async fn list_url(
    State(state): State<AppState>,
    Path(codigo): Path<String>,
) -> impl IntoResponse {
    let map = state.db.lock().unwrap();

    let url = map.get(&codigo).map(|entry| entry.url.clone());

    Json(json!({ "url_original": url }))
}
