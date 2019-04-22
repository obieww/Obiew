var path = require("path");
var webpack = require("webpack");

module.exports = {
    resolve: {
        alias: {
            '@': path.resolve(__dirname, "..", "src")
        }
    },
    plugins: [
        new webpack.ProvidePlugin({
            jQuery: "jquery",
            $: "jquery"
        })
    ]
};