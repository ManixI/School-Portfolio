use hw_4::{read_input, get_max_value};

fn main() {
    let (items, weight) = read_input();
    let (val, items) = get_max_value(items, weight);
    // WARNING: will error if no possible combo will fit in inventory
    let list: String = items.iter().map(|i| i.to_str()).collect();
    let weight = items.iter().fold(0, |acc, i| acc + i.get_weight());
    println!("{}final weight: {}\r\nfinal value: {}\r", list, weight, val);
}
