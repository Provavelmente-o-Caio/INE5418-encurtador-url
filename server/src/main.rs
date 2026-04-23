use axum::{
    Router,
    routing::{delete, get, post},
};

pub mod config;
pub mod data;
mod handlers;

use config::Config;
use data::{AppState, load_database};
use handlers::*;

#[tokio::main]
async fn main() {
    let db = load_database();
    let config = Config::load();

    let app = Router::new()
        .route("/urls", post(shorten_url))
        .route("/urls", get(list_all_urls))
        .route("/urls/{code}", delete(delete_url))
        .route("/urls/{code}", get(list_url))
        .with_state(AppState {
            db,
            host: config.host.clone(),
        });

    let listener = tokio::net::TcpListener::bind("0.0.0.0:3000").await.unwrap();

    axum::serve(listener, app).await.unwrap();
}
