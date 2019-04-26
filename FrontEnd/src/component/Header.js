import React, {Component} from "react";
import "./Header.less";

class Header extends Component {
  render() {
    const {
      page,
      username,
      onChangePage,
      onChangeUser,
    } = this.props;
    return (
      <nav className="main-navigation">
        <div className="rightContent">
          <div className="container">
            <div
              className={"nav-item" + (page === 'home' ? ' active' : '')}
              onClick={() => {
                if (username) {
                  onChangePage('home');
                }
              }}>
              <span className="glyphicon glyphicon-home"></span> Home
            </div>
            <div
              className={"nav-item" + (page === 'register' ? ' active' : '')}
              onClick={() => onChangePage('register')}>
              <span className="glyphicon glyphicon-registration-mark"></span> Register
            </div>
             <div
              className={"nav-item" + (page === 'login' ? ' active' : '')}
              onClick={() => onChangePage('login')}>
              <span className="glyphicon glyphicon-log-in"></span> Login
            </div>
            <div
              className="nav-item"
              onClick={() => {
                onChangePage('login');
                onChangeUser('', '');
              }}>
              <span className="glyphicon glyphicon-log-out"></span> Logout
            </div>
            <div className="left">
              <li>
                <span className="glyphicon glyphicon-user"></span> {username}
              </li>
            </div>
          </div>
        </div>
      </nav>
    );
  }
}

export default Header;