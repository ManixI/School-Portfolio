//#![feature(int_roundings)]

use std::fs::File;
use std::io::{self, BufRead, Read};
use std::path::Path;

pub mod scapegoat;
use btree::BTree;
use scapegoat::SGTree;

pub mod btree;


fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn parse_num(s: &[u8]) -> u32 {
    // TODO: I can speed this up with simd?
    s.bytes().fold(0, |acc, cur| (acc*10) + (cur.unwrap() & 0x0f) as u32)
}

pub fn read_file_sgt(filepath: &str) -> SGTree {
    let mut root = SGTree::new();
    if let Ok(lines) = read_lines(filepath) {
        for line in lines {
            let line = line.unwrap();
            let mut line = line.split_ascii_whitespace();
            let data = scapegoat::NodeData::new(
                line.next().unwrap().to_string(),
                parse_num(line.next().unwrap().as_bytes()),
                parse_num(line.next().unwrap().as_bytes()) as u64
            );
            root.insert(data);
        }
    }    
    root
}

pub fn build_out_from_file(filepath: &str, tree: &SGTree) -> String {
    let mut out = String::new();
    if let Ok(lines) = read_lines(filepath) {
        for line in lines {
            let line = line.unwrap();
            let data = tree.get(&line);
            match data {
                Some(d) => out.push_str(&d.to_str()),
                None => out.push_str(&format!("{} is not currently banned from any servers.\n", line)),
            }
        }
    }
    out
}

pub fn read_file_btree(filepath: &str) -> BTree {
    let mut root = BTree::new();
    if let Ok(lines) = read_lines(filepath) {
        for line in lines {
            let line = line.unwrap();
            let mut line = line.split_ascii_whitespace();
            let data = btree::NodeData::new(
                line.next().unwrap().to_string(),
                parse_num(line.next().unwrap().as_bytes()),
                parse_num(line.next().unwrap().as_bytes()) as u64
            );
            root.insert(data);
        }
    }    
    root
}

pub fn build_out_from_file_btree(filepath: &str, tree: &BTree) -> String {
    let mut out = String::new();
    if let Ok(lines) = read_lines(filepath) {
        for line in lines {
            let line = line.unwrap();
            let data = tree.root.as_ref().unwrap().get(&line);
            match data {
                Some(d) => out.push_str(&d.to_str()),
                None => out.push_str(&format!("{} is not currently banned from any servers.\n", line)),
            }
        }
    }
    out
}