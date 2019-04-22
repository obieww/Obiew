import React, {Component} from 'react';
import {BrowserRouter as Router, Route} from "react-router-dom";
import Header from "@/components/header/Header.js";
import MainPanel from "@/components/body/MainPanel.js";
import Register from "@/components/user/Register.js";
import Login from "@/components/user/Login.js";
import './App.less';

class App extends Component {
  render() {
    return (
      <Router>
        <div className="page">
          <Header username="Anonymous" />
          <div className="content">
            <div className="container main-container">
              <Route path="/" exact component={MainPanel}></Route>
              <Route path="/register" exact component={Register}></Route>
              <Route path="/login" exact component={Login}></Route>
            </div>
          </div>
        </div>
      </Router>
    );
  }
}

export default App;
