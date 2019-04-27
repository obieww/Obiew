import React, {Component} from 'react';
import { connect } from 'react-redux';
import Header from "./HeaderContainer.js";
import MainPanel from "./MainPanelContainer.js";
import Register from "../component/user/Register.js";
import Login from "../component/user/Login.js";
import './App.less';

class App extends Component {
  render() {
    const {
      page,
    } = this.props
    return (
      <div className="page">
        <Header />
        <div className="content">
          {
            page === 'home' ?
              <MainPanel /> :
              page === 'register' ?
              <Register /> :
              <Login />
          }
        </div>
      </div>
    )
  }
}

const mapStateToProps = state => ({
  obiews: state.obiews,
  page: state.page,
})

export default connect(mapStateToProps)(App)
