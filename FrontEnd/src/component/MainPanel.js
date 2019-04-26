import React, {Component} from "react";
import Post from "./card/Post.js";
import Card from "../container/CardContainer.js";
import Portrait from '../images/person_default.png';
import { connect } from 'react-redux';
import {
  createNewObiew,
} from '../store/actions';
import "./MainPanel.less";

class MainPanel extends Component {
  constructor(props) {
    super(props);
    this.state = {
      // hasNewMsg: true,
      showAnchor: false,
    };
    // this.onShowNewMessage = this.onShowNewMessage.bind(this);
    this.onMousewheel = this.onMousewheel.bind(this);
    this.onCreateNewObiew = this.onCreateNewObiew.bind(this);
  }

  componentWillMount() {
    window.addEventListener("scroll", this.onMousewheel);
  }

  componentWillUnmount() {
    window.removeEventListener("scroll", this.onMousewheel);
  }

  onMousewheel() {
    this.setState({
      showAnchor: window.scrollY >= 50
    });
  }

  // onShowNewMessage(e) {
  //   let obiews = this.props.obiews.slice(0);
  //   obiews.unshift(utils.defaultObiew(obiews, "new message"))
  //   this.setState({
  //     obiews: obiews
  //   })
  //   this.setState({
  //     hasNewMsg: false
  //   });
  //   window.scrollTo(0, 0);
  // }

  onCreateNewObiew(msg) {
    const {
      userId,
      obiews,
      onCreateNewObiew,
    } = this.props;
    fetch('https://sleepy-island-43632.herokuapp.com//api/obiew/post', {
      method: 'post',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({
        content: msg,
        user: {
          userId: userId,
        }
      })
    })
    .then(response => response.json())
    .then(response => {
      console.log(response)
    })
    .catch(err => console.log(err));
    
    onCreateNewObiew({
      userId: obiews.length + 1,
      obiewId: obiews.length + 1,
      pic: "../../images/person_default.png",
      username: "Anonymous",
      timestamp: Date.now(),
      hasReobiewed: false,
      reobiews: [],
      likes: [], 
      comments: [], 
      obiew: msg,
    });
    this.setState({
      inputMessage: ""
    });
  }

  render() {
    const {
      showAnchor,
    } = this.state;
    const {
      obiews,
      username,
      profile_portrait,
    } = this.props;
    return (
      <div className="container main-container">
        <div className="main-content">
          <div className="row">
            <div className="col-md-2 right-panel br3 ba dark-gray b--black-10 shadow-5">
              <div className="center">
                <div className="tc">
                  <div className='tc'>
                    <input className='upload-file f4 pa2 w-70 center' type='tex' />
                    <img src={Portrait} className="br-100 h4 w4 dib ba b--black-05 pa2" alt="profile" />
                  </div>
                  <h1 className="f3 mb2">{username}</h1>
                  <h2 className="f5 fw4 gray mt0">{"obiews: " + obiews.length}</h2>
                </div>
              </div>
            </div>
            <div className="col-md-8">
              <Post
                msg="Tell the world what's happening"
                onClick={this.onCreateNewObiew} />
              {/*
                this.state.hasNewMsg ?
                <div className='panel-info panel-heading new-info' onClick={this.onShowNewMessage}>
                  <span>See more obiews</span>
                </div> :
                null
              */}
              {
                obiews.map(obiew => <Card key={obiew.obiewId} obiew={obiew} />)
              }
              {
                showAnchor ?
                <div
                  className="anchor" 
                  v-show="showAnchor"
                  onClick={() => window.scrollTo(0, 0)}>
                  <span className="glyphicon glyphicon-chevron-up" />
                </div> :
                null
              }
            </div>
          </div>
        </div>
      </div>
    );
  }
}

const mapStateToProps = state => ({
  obiews: state.obiews,
  username: state.username,
  userId: state.userId,
});

const mapDispatchToProps = dispatch => ({
  onCreateNewObiew: obiew => dispatch(createNewObiew(obiew)),
});

export default connect(mapStateToProps, mapDispatchToProps)(MainPanel)