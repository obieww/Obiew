import React from 'react';
import ReactDOM from 'react-dom';
import App from './container/App.js';
import 'jquery/src/jquery';
import "bootstrap/dist/css/bootstrap.css";
import "bootstrap/dist/js/bootstrap.js";
import 'tachyons';

import { Provider } from 'react-redux';
import store from './store/store';

ReactDOM.render(
  <Provider store={store}>
    <App />
  </Provider>, 
  document.getElementById('root'));