use std::result::Result;

pub struct Config {
    pub path: String,
    pub pattern: String,
}

impl Config {
    pub fn new(argv: &[String]) -> Result<Config, &'static str> {
        let mut path = String::from("./");
        let mut pattern: String;

        let argc = argv.len();

        if argc == 1 {
            return Err("not enough arguments");
        } else if argc == 2 {
            pattern = argv[1].clone();
        } else if argc == 3 {
            path = argv[1].clone();
            pattern = argv[2].clone();
        } else {
            return Err("invalid arguments");
        }

        Ok(Config { path, pattern })
    }
}
