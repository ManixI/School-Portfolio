use criterion::{criterion_group, criterion_main, Criterion, BenchmarkId};
use hw_2::*;

const FILE_PATH: &str = "/home/ian/dev/cs-450/hw-2/src/players.dat";

pub fn criterion_benchmark(c: &mut Criterion) {
    let input = read_file(FILE_PATH);

    let mut group = c.benchmark_group("test score");
    group.significance_level(0.05).sample_size(10);
    /*group.bench_with_input(BenchmarkId::new("read from file", "players.dat"), FILE_PATH, 
        |b, s| {
            b.iter(|| read_file(s));
        });
    group.bench_with_input(BenchmarkId::new("read from file opt", "players.dat"), FILE_PATH, 
        |b, s| {
            b.iter(|| read_file_opt(s));
        });*/
    group.bench_with_input(BenchmarkId::new("sort_basic", "players.dat"), &input, 
        |b, s| {
            b.iter(|| default_sort(&mut s.clone()));
        });
    /*group.bench_with_input(BenchmarkId::new("quicksort", "players.dat"), &input, 
        |b, s| {
            b.iter(|| quick_sort(&mut Vec!(s.clone())));
        });*/
    group.bench_with_input(BenchmarkId::new("count-quick combo", "players.dat"), &input, 
        |b, s| {
            b.iter(|| count_sort_partle(s.clone()));
        });
    group.bench_with_input(BenchmarkId::new("count-quick combo threaded", "players.dat"), &input, 
        |b, s| {
            b.iter(|| count_sort_partle_threaded(s.clone()));
        });
    group.finish();  
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);