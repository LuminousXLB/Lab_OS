use std::env;
use std::fs;
use std::path::Path;
use std::process::exit;
use std::result::Result;

use treeme::Config;

fn usage(executable: &String) {
    println!("\nUsage: {} [PATH]", executable);
    println!("Print the tree view of PATH");
    println!("Example: {}", executable);
    println!("         {} /home/wang/work", executable);
}

fn main() {
    let argv: Vec<String> = env::args().collect();
    let config = Config::new(&argv).unwrap_or_else(|err| {
        eprintln!("Problem parsing arguments: {}", err);
        usage(&argv[0]);
        exit(1);
    });

    if let Err(e) = treeme(&Path::new(&config.path), &String::from("")) {
        eprintln!("Problem reading file: {}", e);
        exit(1);
    };
}

fn treeme(path: &Path, indent: &String) -> Result<(), std::io::Error> {
    let _attr = fs::metadata(path).unwrap_or_else(|err| {
        eprintln!("Error occurs when reading {}: {}", &path.display(), err);
        exit(1);
    });

    let mut next_level_indent = indent.clone();
    next_level_indent.push_str("    ");

    print!(
        "{}{}",
        &indent,
        path.file_name().unwrap_or_default().to_string_lossy()
    );

    if path.is_dir() {
        println!("/");
        for entry in fs::read_dir(path)? {
            let cpath = entry.unwrap().path();
            if let Err(e) = treeme(&cpath, &next_level_indent) {
                eprintln!("Error occurs when scanning {}: {}", path.display(), e);
            }
        }
    } else {
        println!();
    }

    Ok(())
}
