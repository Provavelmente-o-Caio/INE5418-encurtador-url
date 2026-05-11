use std::env;

pub mod config;
pub mod urlshort_client;

use config::Config;
use urlshort_client::UrlShortClient;

fn print_usage(program: &str) {
    eprintln!(
        "Uso:\n\
         {0} encurta <url_original>\n\
         {0} resolve <codigo_curto>\n\
         {0} remove <codigo_curto>\n\
         {0} lista\n",
        program
    );
}

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        print_usage(&args[0]);
        std::process::exit(1);
    }

    let cfg = Config::load();
    let client = UrlShortClient::new(cfg);

    let op = &args[1];

    match op.as_str() {
        "encurta" => {
            if args.len() < 3 {
                print_usage(&args[0]);
                std::process::exit(1);
            }
            let arg = &args[2];
            match client.encurta(arg) {
                Ok(codigo) => {
                    println!("URL encurtada com sucesso!");
                    println!("  Original : {}", arg);
                    println!("  Código   : {}", codigo);
                }
                Err(e) => {
                    eprintln!("[ERRO] encurta falhou: {}", e);
                    std::process::exit(1);
                }
            }
        }

        "resolve" => {
            if args.len() >= 3 {
                let arg = &args[2];
                match client.resolve(arg) {
                    Ok(url) => {
                        println!("Código resolvido com sucesso!");
                        println!("  Código : {}", arg);
                        println!("  URL    : {}", url);
                    }
                    Err(e) => {
                        eprintln!("[ERRO] resolve falhou: {}", e);
                        std::process::exit(1);
                    }
                }
            } else {
                print_usage(&args[0]);
                std::process::exit(1);
            }
        }

        "lista" => match client.lista() {
            Ok(list) => {
                println!("Lista de URLs:");
                println!("{}", list);
            }
            Err(e) => {
                eprintln!("[ERRO] lista falhou: {}", e);
                std::process::exit(1);
            }
        },

        "remove" => {
            if args.len() < 3 {
                print_usage(&args[0]);
                std::process::exit(1);
            }
            let arg = &args[2];
            match client.remove_url(arg) {
                Ok(()) => {
                    println!("Mapeamento removido com sucesso!");
                    println!("  Código : {}", arg);
                }
                Err(e) => {
                    eprintln!("[ERRO] remove falhou: {}", e);
                    std::process::exit(1);
                }
            }
        }

        _ => {
            eprintln!("Operação desconhecida: {}", op);
            print_usage(&args[0]);
            std::process::exit(1);
        }
    }
}
