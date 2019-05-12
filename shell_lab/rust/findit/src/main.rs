use std::env;
use std::fs;
use std::path::Path;
use std::process::exit;
use std::result::Result;

use findit::Config;

fn usage(executable: &String) {
    println!("\nUsage: {} [PATH] PATTERN", executable);
    println!("Search for PATTERN in each of the files in PATH");
    println!("Example: {} /home/wang/work searchstring", executable);
}

fn main() {
    let argv: Vec<String> = env::args().collect();
    let config = Config::new(&argv).unwrap_or_else(|err| {
        eprintln!("Problem parsing arguments: {}", err);
        usage(&argv[0]);
        exit(1);
    });

    if let Err(e) = findit(&Path::new(&config.path), &config.pattern) {
        eprintln!("Problem reading file: {}", e);
        exit(1);
    };
}

fn findit(path: &Path, pattern: &String) -> Result<(), std::io::Error> {
    let _attr = fs::metadata(path).unwrap_or_else(|err| {
        eprintln!("Error occurs when reading {}: {}", &path.display(), err);
        exit(1);
    });

    if path.is_dir() {
        for entry in fs::read_dir(path)? {
            let cpath = entry.unwrap().path();
            if let Err(e) = findit(&cpath, pattern) {
                eprintln!("Error occurs when scanning {}: {}", path.display(), e);
            }
        }
    } else if path.is_file() {
        let ext = path.extension().unwrap_or_default();
        if ext == "c" || ext == "h" {
            if let Err(e) = do_search(path, pattern) {
                eprintln!("Error occours when processing {}: {}", path.display(), e);
            }
        }
    }

    Ok(())
}

fn do_search(path: &Path, pattern: &String) -> Result<(), std::io::Error> {
    let buffer = fs::read(path)?;
    let contents = String::from_utf8_lossy(&buffer);

    for line in contents.lines().enumerate() {
        if (line.1).contains(pattern) {
            println!("In {} L{}:", path.display(), line.0);
            println!("\t{}", line.1)
        }
    }

    Ok(())
}
