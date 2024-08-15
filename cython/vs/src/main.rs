use axum::{routing::get, Router};

use crate::Config;

#[tokio::main]
async fn main() {
    // build our application with a route
    let app = Router::new()
        // `GET /` goes to `root`
        .route("/test", get(root));

    // run our app with hyper, listening globally on port 3000
    let listener = tokio::net::TcpListener::bind("0.0.0.0:8888").await.unwrap();
    axum::serve(listener, app).await.unwrap();
}

// basic handler that responds with a static string
async fn root() -> &'static str {
    "test"
}

#[cfg(test)]
mod test_main {
    use super::*;

    #[test]
    fn test_1() {
        let mut a = Box::new(1);

        *a = 2;
    }
}
