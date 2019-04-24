import React, {Component} from "react";
import utils from "@/utils";
import "./Obiew.less";

class Obiew extends Component {
  render() {
    return (
      <div className="media">
        <div className="media-left">
          <img className="media-object person-icon" src={require("../../images/person_default.png")} alt="..."/>
        </div>
        <div className="media-body">
          <p><b className="remark-person-name">{this.props.info.username}</b></p>
          <p><span>{this.props.info.obiew}</span></p>
          <div className="remark-foot">
            <div className="remark-item-left">{utils.timeFormat(this.props.info.timestamp)}</div>
            {
              this.props.info.rightBtns.map((rightBtn, index) => (
                <React.Fragment key={index}>
                  <div className="remark-item-right" onClick={(e) => this.props.onClickRightBtn(rightBtn)}>
                    <span className={"glyphicon " + rightBtn.icon}></span>  {rightBtn.label}
                  </div>
                  {
                    index !== this.props.info.rightBtns.length - 1 ? 
                    <div className="remark-item-right split" /> :
                    null
                  }
                </React.Fragment>
              ))
            }
          </div>
        </div>
      </div>
    );
  }
}

export default Obiew;