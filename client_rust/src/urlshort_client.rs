use std::{
    io::{Read, Write},
    net::TcpStream,
};

use crate::config::Config;

const BUF_SIZE: usize = 1024;

pub struct UrlShortClient {
    host: String,
    port: u16,
}

impl UrlShortClient {
    pub fn new(cfg: Config) -> Self {
        Self {
            host: cfg.host,
            port: cfg.port,
        }
    }

    /* ── Abre conexão TCP com o proxy ─────────────────────── */
    fn conecta(&self, msg: &str) -> Result<String, UrlShortError> {
        let addr = format!("{}:{}", self.host, self.port);

        let mut stream = TcpStream::connect(addr).map_err(|_| UrlShortError::Connect)?;

        stream
            .write_all(msg.as_bytes())
            .map_err(|_| UrlShortError::Send)?;

        let mut buf = [0u8; BUF_SIZE];
        stream.read(&mut buf).map_err(|_| UrlShortError::Recv)?;
        let response = String::from_utf8_lossy(&buf).to_string();

        Ok(response)
    }

    fn parse_response(response: &str) -> Result<String, UrlShortError> {
        if let Some(payload) = response.strip_prefix("OK") {
            Ok(payload.trim_end().to_string())
        } else if let Some(payload) = response.strip_prefix("ERR") {
            Err(UrlShortError::Server(payload.trim_end().to_string()))
        } else {
            Err(UrlShortError::Proto)
        }
    }

    /* ── encurta() ────────────────────────────────────────── */
    pub fn encurta(&self, url: &str) -> Result<String, UrlShortError> {
        if url.is_empty() {
            return Err(UrlShortError::Arg);
        }
        let resp = self.conecta(&format!("POST {}", url))?;
        Self::parse_response(&resp)
    }

    /* ── resolve() ────────────────────────────────────────── */
    pub fn resolve(&self, short_url: &str) -> Result<String, UrlShortError> {
        if short_url.is_empty() {
            return Err(UrlShortError::Arg);
        }
        let resp = self.conecta(&format!("GET {}", short_url))?;
        Self::parse_response(&resp)
    }

    /* ── remove_url() ─────────────────────────────────────── */
    pub fn remove_url(&self, short_url: &str) -> Result<(), UrlShortError> {
        if short_url.is_empty() {
            return Err(UrlShortError::Arg);
        }
        let resp = self.conecta(&format!("DELETE {}", short_url))?;
        Self::parse_response(&resp).map(|_| ())
    }
}

/* ── Mensagens de erro ────────────────────────────────── */
#[derive(Debug)]
pub enum UrlShortError {
    OK,
    Socket,
    Connect,
    Send,
    Recv,
    Proto,
    Arg,
    Server(String),
}

impl std::fmt::Display for UrlShortError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            UrlShortError::OK => write!(f, "Sucesso"),
            UrlShortError::Socket => write!(f, "Falha ao criar socket"),
            UrlShortError::Connect => write!(f, "Falha ao conectar ao proxy"),
            UrlShortError::Send => write!(f, "Falha ao enviar dados"),
            UrlShortError::Recv => write!(f, "Falha ao recever dados"),
            UrlShortError::Proto => write!(f, "Resposta inesperada do proxy"),
            UrlShortError::Arg => write!(f, "Argumento inválido"),
            UrlShortError::Server(msg) => write!(f, "Proxy retornou erro: {}", msg),
        }
    }
}
