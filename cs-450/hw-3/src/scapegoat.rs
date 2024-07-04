use std::cmp::Ordering;

const ALPHA: f32 = 0.7;

#[derive(Debug, PartialEq, Clone)]
pub struct NodeData {
    name: String,
    servers: Vec<u32>,
    timestamp: u64,
}

impl NodeData {
    pub fn new(name: String, server: u32, timestamp: u64) -> Self {
        let mut servers = Vec::new();
        servers.push(server);
        let mut timestamps = Vec::new();
        timestamps.push(timestamp);
        NodeData {
            name,
            servers,
            timestamp
        }
    }

    pub fn to_str(&self) -> String {
        format!("{} was banned from {} servers. most recently on: {}\n",
            self.name,
            self.servers.len(),
            self.timestamp
        )
    }

    pub fn increment(&mut self, data: &Self) {
        let server = data.servers[0];
        let timestamp = data.timestamp;
        if timestamp > self.timestamp {
            self.timestamp = timestamp;
        }
        if !self.servers.contains(&server) {
            self.servers.push(server);
        }
    }
}



#[derive(Debug, PartialEq)]
pub struct Node {
    data: NodeData,
    left: Option<usize>,
    right: Option<usize>,
    parent: Option<usize>,
}

#[derive(Default)]
pub struct SGTree {
    root: Option<usize>,
    tree: Vec<Node>,
}

impl SGTree {
    pub fn new() -> Self {
        SGTree {
            root: None,
            tree: Vec::<Node>::new(),
        }
    }

    pub fn print_all(&self) {
        let mut out = String::new();
        self.print_helper(0, &mut out);
        println!("{}", out);
    }

    pub fn print_helper(&self, index: usize, out: &mut String) {
        if self.tree[index].left.is_some() {
            self.print_helper(self.tree[index].left.unwrap(), out);
        }
        out.push_str(&self.tree[index].data.to_str());
        if self.tree[index].right.is_some() {
            self.print_helper(self.tree[index].right.unwrap(), out);
        }
    }

    pub fn size(&self, index: usize) -> usize {
        let node = &self.tree[index];
        match (node.right, node.left) {
            (None, None) => 1,
            (Some(right), None) => 1 + self.size(right),
            (None, Some(left)) => 1 + self.size(left),
            (Some(right), Some(left)) => 1 + self.size(right) + self.size(left),
        }
    }

    pub fn get(&self, name: &String) -> Option<&NodeData> {
        let mut index = self.root;
        loop {
            match self.tree[index.unwrap()].data.name.cmp(name) {        
                Ordering::Equal => return Some(&self.tree[index.unwrap()].data),
                Ordering::Less => {
                    match self.tree[index.unwrap()].right {
                        None => return None,
                        Some(val) => index = Some(val),
                    };
                },
                Ordering::Greater => {
                    match self.tree[index.unwrap()].left {
                        None => return None,
                        Some(val) => index = Some(val),
                    };
                }
            }
        }
    }

    fn print_tree(&self, index: usize) {
        println!("node: {}, parent: {:?}, left: {:?}, right: {:?}",
            index, 
            self.tree[index].parent,
            self.tree[index].left,
            self.tree[index].right
        );
        if self.tree[index].left.is_some() {
            self.print_tree(self.tree[index].left.unwrap())
        }
        
        if self.tree[index].right.is_some() {
            self.print_tree(self.tree[index].right.unwrap())
        }
    }

    pub fn insert(&mut self, data: NodeData) {
        if self.root.is_none() {
            self.root = Some(0);
            self.tree.push(Node::new(data));
        } else {
            let mut depth = 1;
            let mut index = 0;
            
            loop {
                match self.tree[index].data.name.cmp(&data.name) {
                    Ordering::Equal => {
                        self.tree[index].data.increment(&data);
                        return;
                    }
                    Ordering::Less => {
                        depth += 1;
                        if self.tree[index].right.is_none() {
                            let mut node = Node::new(data);
                            node.parent = Some(index);
                            self.tree.push(node);
                            self.tree[index].right = Some(self.tree.len()-1);
                            break;
                        } else {
                            index = *self.tree[index].right.as_ref().unwrap();
                        }
                    }
                    Ordering::Greater => {
                        depth += 1;
                        if self.tree[index].left.is_none() {
                            let mut node = Node::new(data);
                            node.parent = Some(index);
                            self.tree.push(node);
                            self.tree[index].left = Some(self.tree.len()-1);
                            break;
                        } else {
                            index = *self.tree[index].left.as_ref().unwrap();
                        }
                    }
                }
            }
            if depth as f32 > (self.tree.len() as f32).log(1.0/ALPHA) + 1.0 {
                // tree is unbalanced
                let mut largest_i = 2.0;
                let mut i = 2.0;
                let mut index = self.tree.len()-1;
                // tree needs a depth of at least 3 before rebalancing is a possibility
                index = self.tree[index].parent.unwrap();
                index = self.tree[index].parent.unwrap();
                let mut size = self.size(index);
                let mut largest_index = index;
                while index != self.root.unwrap() {
                    if i > (size as f32).log(1.0/ALPHA) + 1.0 {
                        //self.rebalance(self.root.unwrap().clone());
                        //break;

                        if i > largest_i {
                            largest_i = i;
                            largest_index = index;
                        } else {
                            break;
                        }
                        if self.tree[index].parent.is_none() {
                            continue;
                        } else if self.tree[self.tree[index].parent.unwrap()].left.is_some()
                                && self.tree[self.tree[index].parent.unwrap()].right.is_some() {
                            if index == self.tree[self.tree[index].parent.unwrap()].left.unwrap() {
                                size += self.size(self.tree[self.tree[index].parent.unwrap()].right.unwrap()) + 1;
                            } else {
                                size += self.size(self.tree[self.tree[index].parent.unwrap()].left.unwrap()) + 1;
                            }
                        } else {
                            size += 1;
                        }
                        
                    }
                    index = self.tree[index].parent.unwrap();
                    i += 1.0;
                }
                self.rebalance(largest_index);
            }
        }
    }

    fn rebalance(&mut self, index: usize) {
        let parent_index = self.tree[index].parent;
        let mut arr = Vec::<usize>::with_capacity(self.size(index));
        self.flatten(index, &mut arr);
        for i in &arr {
            self.tree[*i].left = None;
            self.tree[*i].right = None;
            self.tree[*i].parent = None;
        }

        let len = arr.len();
        let node_index = self.rebuild_vec_2(&mut arr, 0, len).unwrap();

        self.tree[node_index].parent = parent_index;
        if parent_index.is_some() {
            let parent_index = parent_index.unwrap();
            if self.tree[parent_index].left.unwrap() == index {
                self.tree[parent_index].left = Some(node_index);
            } else {
                self.tree[parent_index].right = Some(node_index);
            }
        } else {
            self.root = Some(node_index);
        }
    } 

    fn flatten(&self, index: usize, arr: &mut Vec<usize>) {
        let node = &self.tree[index];
        if node.left.is_some() {
            self.flatten(node.left.unwrap(), arr);
        }
        arr.push(index);
        if node.right.is_some() {
            self.flatten(node.right.unwrap(), arr);
        }
    }

    fn rebuild_vec_2(&mut self, arr: &mut Vec<usize>, low: usize, high: usize) -> Option<usize> {
        if high == 0 {
            return None;
        }

        let mid = high / 2;

        self.tree[arr[low + mid]].left = self.rebuild_vec_2(arr, low, mid);
        if self.tree[arr[low + mid]].left.is_some() {
            self.set_parent(self.tree[arr[low + mid]].left.unwrap(), Some(arr[low+mid]));
        }

        self.tree[arr[low+mid]].right = self.rebuild_vec_2(arr, low+mid+1, high-mid-1);
        if self.tree[arr[low + mid]].right.is_some() {
            self.set_parent(self.tree[arr[low + mid]].right.unwrap(), Some(arr[low+mid]));
        }

        Some(arr[low+mid])

    }

    fn set_parent(&mut self, target: usize, parent: Option<usize>) {
        self.tree[target].parent = parent;
    }
}

impl Node {
    pub fn new(data: NodeData) -> Self {
        Node {
            data,
            left: None,
            right: None,
            parent: None,
        }
    }
}
