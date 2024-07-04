#![feature(ascii_char)]

use std::io::{stdin, Read};
use std::cmp::max;
use std::str::from_utf8_unchecked;

const NAME_MAX_LEN: usize = 127;
const ITEMS_MAX_NUM: usize = 128;
//const INVENTORY_MAX_WEIGHT: usize = 1200;

#[derive(PartialEq, Eq, Hash, Clone, Copy, Debug)]
pub struct Item {
    weight: usize,
    value: usize,
    name: [u8; NAME_MAX_LEN],
}
impl Item {
    fn new(name: [u8; NAME_MAX_LEN], weight: usize, value: usize) -> Self {
        Item {
            name,
            weight,
            value
        }
    }

    pub fn to_str(&self) -> String {
        unsafe {
            let name = from_utf8_unchecked(&self.name);
            let name = name.split(|c| c=='\0').next().unwrap(); 
            let out = format!("{}, {}, {}\r\n", name, self.weight, self.value);
            // this avoids the output file starting with a \n
            out[1..out.len()].to_string()
        }
    }

    pub fn get_weight(&self) -> usize {
        self.weight
    }
}

fn parse_num(s: &[u8]) -> usize {
    // TODO: I can speed this up with simd?
    s.bytes().fold(0, |acc, cur| (acc*10) + (cur.unwrap() & 0x0f) as usize)
}

// ref: https://stackoverflow.com/questions/28219231/how-to-idiomatically-copy-a-slice
fn copy_slice(dst: &mut [u8], src: &[u8]) -> usize {
    let mut c = 0;
    for (d, s) in dst.iter_mut().zip(src.iter()) {
        *d = *s;
        c += 1;
    }
    c 
}

pub fn read_input() -> (Vec<Item>, usize) {
    let mut input = stdin().lock();
    let mut input_buf = Vec::<u8>::with_capacity(ITEMS_MAX_NUM * (NAME_MAX_LEN + 10 + 10));
    let _ = input.read_to_end(&mut input_buf);

    let mut lines_iter = input_buf.split(|c| c==&b'\r');
    let weight = parse_num(lines_iter.next().unwrap());

    let mut items = Vec::<Item>::with_capacity(ITEMS_MAX_NUM);

    for line in lines_iter {
        let mut line_iter = line.split(|c| c==&b';');
        let mut name: [u8; NAME_MAX_LEN] = [b'\0'; NAME_MAX_LEN];
        copy_slice(&mut name, line_iter.next().unwrap());
        let weight = parse_num(line_iter.next().unwrap());
        let value = line_iter.next().unwrap().split(|c| c == &b'\n').next().unwrap();
        let value = parse_num(value);
        
        items.push(Item::new(name, weight, value));
    }

    (items, weight)
}

fn invintory(max_weight: usize, item_list: Vec<Item>) -> (usize, Vec<usize>) {
    let num_items = item_list.len();
    let table: Vec<Vec<usize>> = invintory_table(&max_weight, &item_list);
    let items: Vec<usize> = invintory_items(&item_list, &table, num_items, max_weight);

    (table[num_items][max_weight], items)
}

fn invintory_items(items: &[Item], table: &[Vec<usize>], i: usize, j: usize) -> Vec<usize> {
    if i == 0 {
        return Vec::<usize>::new();
    }
    if table[i][j] > table[i-1][j] {
        let mut invintory = invintory_items(items, table, i-1, j - items[i-1].weight);
        invintory.push(i);
        invintory
    } else {
        invintory_items(items, table, i-1, j)
    }
}

fn invintory_table(max_weight: &usize, items: &[Item]) -> Vec<Vec<usize>> {
    let n = items.len();
    let mut table = vec![vec![0; *max_weight + 1]; n + 1];

    for i in 0..=n {
        for j in 0..=*max_weight {
            if i == 0 || j == 0 {
                table[i][j] = 0;
            } else if items[i-1].weight <= j {
                table[i][j] = max(items[i-1].value + table[i-1][j-items[i-1].weight], table[i-1][j]);
            } else {
                table[i][j] = table[i-1][j];
            }
        }
    }
    table
}

pub fn get_max_value(items: Vec<Item>, weight: usize) ->  (usize, Vec<Item>) {

    let (value, item_list) = invintory(weight, items.clone());
    let mut out = Vec::<Item>::with_capacity(item_list.len());
    for index in item_list {
        out.push(items[index-1]);
    }
    (value, out) 
}
