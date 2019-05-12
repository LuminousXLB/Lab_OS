use std::result::Result;

pub struct Config {
    pub path: String,
}

impl Config {
    pub fn new(argv: &[String]) -> Result<Config, &'static str> {
        let mut path = String::from("./");

        let argc = argv.len();
        if argc == 1 {
            // return Err("not enough arguments");
        } else if argc == 2 {
            path = argv[1].clone();
        } else {
            return Err("invalid arguments");
        }

        Ok(Config { path })
    }
}
