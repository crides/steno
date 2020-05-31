use indicatif::{ProgressBar, ProgressStyle};

pub fn progress_bar(len: usize, msg: &str) -> ProgressBar {
    let bar = ProgressBar::new(len as u64)
        .with_style(ProgressStyle::default_bar()
            .progress_chars("=> ")
            .template("{msg} [{wide_bar}] ETA: {eta} ({bytes}/{total_bytes})"));
    bar.set_draw_delta(len as u64 / 200);
    bar.set_message(msg);
    bar
}
