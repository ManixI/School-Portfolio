/*use std::cmp::Ordering;

const MAX_KEYS: usize = 32;

#[derive(Debug, PartialOrd)]
pub struct NodeData {
    name: String,
    servers: Vec<u32>,
    timestamp: u64,
}

impl Ord for NodeData {
    fn cmp(&self, other: &Self) -> Ordering {
        match self.name.cmp(&other.name) {
            Ordering::Equal => Ordering::Equal,
            Ordering::Greater => Ordering::Greater,
            Ordering::Less => Ordering::Less
        }
    }
}

impl PartialEq for NodeData {
    fn eq(&self, other: &Self) -> bool {
        self.name == other.name
    }

}

impl Eq for NodeData {}

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
    parent: Option<usize>,
    children: Vec<usize>,
    data: Vec<NodeData>
}

impl Node {
    pub fn new(data: Option<NodeData>, parent: Option<usize>) -> Self {
        let mut data_vec = Vec::<NodeData>::with_capacity(MAX_KEYS);
        if data.is_some() {
            data_vec.push(data.unwrap());
        }
        let children_vec = Vec::<usize>::with_capacity(MAX_KEYS);
        Node {
            parent,
            children: children_vec,
            data: data_vec
        }
    }
}

pub struct BTree {
    root: Option<usize>,
    tree: Vec<Node>
}

impl BTree {
    pub fn new() -> Self {
        BTree {
            root: None,
            tree: Vec::<Node>::new()
        }
    }

    pub fn insert(&mut self, data: NodeData) {
        let root = self.root;
        if self.root.is_none() {
            self.root = Some(0);
            self.tree.push(Node::new(Some(data), None));
            return;
        } else if self.tree[root.unwrap()].data.len() == MAX_KEYS {
            // root node is full
            let tmp = Node::new(Some(data), None);
            self.tree.push(tmp);
            let tmp_index = self.tree.len()-1;
            self.root = Some(tmp_index);
            self.insert_child(tmp_index, root.unwrap());
            self.split_child(tmp_index, 0);
        }
        self.insert_not_full(self.root.unwrap(), data);
    }

    fn insert_not_full(&mut self, index: usize, data: NodeData) {
        let mut node = &mut self.tree[index];
        if node.children.len() == 0 {
            // leaf node
            // ref: https://stackoverflow.com/questions/36249693/whats-the-most-efficient-way-to-insert-an-element-into-a-sorted-vector
            match node.data.binary_search(&data) {
                Ok(pos) => node.data[pos].increment(&data),
                Err(pos) => node.data.insert(pos, data),
            }
        } else {
            let mut i = node.data
                .iter()
                .position(|x| x > &data)
                .unwrap_or(node.data.len());

            if node.children.len() == MAX_KEYS {
                self.split_child(node.children[i], i);
                if node.data[0] > self.tree[i].data[0] {
                    i += 1;
                }
                self.insert_not_full(node.children[i], data);
            }
        }
    }

    fn split_child(&mut self, index: usize, split_on: usize) {
        let c_node_index = self.tree[index].children[split_on];
        let new_node = Node::new(None,Some(c_node_index));
        self.tree.push(new_node);
        self.insert_child(split_on + 1, self.tree.len()-1);
    }

    fn insert_child(&mut self, target: usize, child: usize) {
        // TODO: this
    }

    pub fn get(&self, name: &String) -> Option<&NodeData> {
        // TODO: this

        None
    }
}*/

use std::cmp::Ordering;

#[derive(Debug, PartialOrd)]
pub struct NodeData {
    name: String,
    servers: Vec<u32>,
    timestamp: u64,
}

impl Ord for NodeData {
    fn cmp(&self, other: &Self) -> Ordering {
        match self.name.cmp(&other.name) {
            Ordering::Equal => Ordering::Equal,
            Ordering::Greater => Ordering::Greater,
            Ordering::Less => Ordering::Less
        }
    }
}

impl PartialEq for NodeData {
    fn eq(&self, other: &Self) -> bool {
        self.name == other.name
    }

}

impl Eq for NodeData {}

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
    left: Option<Box<Node>>,
    right: Option<Box<Node>>,
}

impl Node {
    pub fn new(data: NodeData) -> Self {
        Node {
            data,
            left: None,
            right: None,
        }
    }

    pub fn get(&self, target: &String) -> Option<&NodeData> {
        match self.data.name.cmp(target) {
            Ordering::Equal => return Some(&self.data),
                Ordering::Greater => {
                    match self.left {
                        Some(ref n) => return n.get(target),
                        None => return None,
                    }
                },
                Ordering::Less => { 
                    match self.right {
                        Some(ref n) => return n.get(target),
                        None => return None,
                    }
                }
        }
    }
}

pub struct BTree {
    pub root: Option<Box<Node>>
}

impl BTree {
    pub fn new() -> BTree {
        BTree { root: None }
    }

    pub fn insert(&mut self, data: NodeData) {
        if self.root.is_none() {
            self.root = Some(Box::new(Node::new(data)));
        } else {
            let mut root = &mut self.root;
            while let Some(ref mut node) = root {
                match node.data.cmp(&data) {
                    Ordering::Equal => {
                        node.data.increment(&data);
                        return;
                    },
                    Ordering::Greater => {
                        if node.left.is_none() {
                            node.left = Some(Box::new(Node::new(data)));
                            return;
                        } else {
                            root = &mut node.left;
                        }
                    }
                    Ordering::Less => {
                        if node.right.is_none() {
                            node.right = Some(Box::new(Node::new(data)));
                            return;
                        } else {
                            root = &mut node.right;
                        }
                    }
                }
            }
        }
    }

    pub fn get(&self, target: &String) -> Option<&NodeData> {
        self.root.as_ref().unwrap().get(target)
    }
}