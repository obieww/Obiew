import React, {Component} from "react";
import {BrowserRouter as Router, Route, Link} from "react-router-dom";
import store from "@/store";
import "./Header.less";

const CustomerLink = ({label, to, className}) => (
  <Route path={to} exact={true} children={({match}) => (
    <Link className={match ? "nav-item active" : "nav-item"} to={to}>
      <span className={"glyphicon " + className}></span> {label}
    </Link>
  )}/>
);

class Header extends Component {
  render() {
    return (
      <nav className="main-navigation">
        <div className="rightContent">
          <div className="container">
            <CustomerLink to="/" label="Home" className="glyphicon-home"></CustomerLink>
            <CustomerLink to="/register" label="Register" className="glyphicon-registration-mark"></CustomerLink>
            <CustomerLink to="/login" label="Login" className="glyphicon-log-in"></CustomerLink>
            <div className="nav-item">
              <span className={"glyphicon glyphicon-log-out"}></span> Logout
            </div>
            <div className="left">
              <li>
                <span className="glyphicon glyphicon-user"></span> {this.props.username}
              </li>
            </div>
          </div>
        </div>
      </nav>
    );
  }
}

export default Header;